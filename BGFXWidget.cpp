#include <cassert>
#include <fstream>
#include <array>

#include <QResizeEvent>
#include <QMouseEvent>
#include <QWheelEvent>

#include <bgfx/embedded_shader.h>
#include <bgfx/platform.h>

#include "BGFXWidget.h"

#include <vs_cubes.sc.glsl.bin.h>
#include <vs_cubes.sc.essl.bin.h>
#include <vs_cubes.sc.spv.bin.h>
#include <fs_cubes.sc.glsl.bin.h>
#include <fs_cubes.sc.essl.bin.h>
#include <fs_cubes.sc.spv.bin.h>
#if defined(_WIN32)
#include <vs_cubes.sc.dx10.bin.h>
#include <vs_cubes.sc.dx11.bin.h>
#include <fs_cubes.sc.dx10.bin.h>
#include <fs_cubes.sc.dx11.bin.h>
#endif //  defined(_WIN32)
#if __APPLE__
#include <vs_cubes.sc.mtl.bin.h>
#include <fs_cubes.sc.mtl.bin.h>
#endif // __APPLE__

// Embedded shader has DirectX 11 enabled for Linux
#ifdef __linux__
#undef BGFX_EMBEDDED_SHADER_DXBC
#define BGFX_EMBEDDED_SHADER_DXBC(...)
#endif

static const bgfx::EmbeddedShader s_embeddedShaders[] =
{
	BGFX_EMBEDDED_SHADER(vs_cubes),
	BGFX_EMBEDDED_SHADER(fs_cubes),

	BGFX_EMBEDDED_SHADER_END()
};

struct PosColorVertex
{
    float m_x;
    float m_y;
    float m_z;
    uint32_t m_abgr;

    static void init()
    {
        ms_layout
            .begin()
            .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
            .end();
    };

    static bgfx::VertexLayout ms_layout;
};

bgfx::VertexLayout PosColorVertex::ms_layout;

static PosColorVertex s_cubeVertices[] =
{
    {-1.0f,  1.0f,  1.0f, 0xff000000 },
    { 1.0f,  1.0f,  1.0f, 0xff0000ff },
    {-1.0f, -1.0f,  1.0f, 0xff00ff00 },
    { 1.0f, -1.0f,  1.0f, 0xff00ffff },
    {-1.0f,  1.0f, -1.0f, 0xffff0000 },
    { 1.0f,  1.0f, -1.0f, 0xffff00ff },
    {-1.0f, -1.0f, -1.0f, 0xffffff00 },
    { 1.0f, -1.0f, -1.0f, 0xffffffff },
};

static const uint16_t s_cubeTriList[] =
{
    0, 1, 2, // 0
    1, 3, 2,
    4, 6, 5, // 2
    5, 6, 7,
    0, 2, 4, // 4
    4, 2, 6,
    1, 5, 3, // 6
    5, 7, 3,
    0, 4, 1, // 8
    4, 5, 1,
    2, 3, 6, // 10
    6, 3, 7,
};

BGFXWidget::BGFXWidget(QWidget *parent) : QWidget(parent)
{
    setDefaultCamera();
}

void BGFXWidget::showEvent(QShowEvent* event)
{
    debug = BGFX_DEBUG_NONE;
    reset = BGFX_RESET_NONE;

    bgfx::Init init;
    init.type = renderer_type;
    init.vendorId = BGFX_PCI_ID_NONE;
    init.resolution.width = realWidth();
    init.resolution.height = realHeight();
    init.resolution.reset = reset;
    init.platformData.nwh = reinterpret_cast<void*>(winId());
    bgfx::init(init);

    // Enable debug text.
    bgfx::setDebug(debug);

    // Set view 0 clear state.
    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x303030ff, 1.0f, 0);

    // Create vertex stream declaration.
    PosColorVertex::init();

    // Create static vertex buffer.
    m_vbh = bgfx::createVertexBuffer(
        // Static data can be passed with bgfx::makeRef
        bgfx::makeRef(s_cubeVertices, sizeof(s_cubeVertices))
        , PosColorVertex::ms_layout
    );

    // Create static index buffer for triangle list rendering.
    m_ibh = bgfx::createIndexBuffer(
        // Static data can be passed with bgfx::makeRef
        bgfx::makeRef(s_cubeTriList, sizeof(s_cubeTriList))
    );

    // Create program from shaders.
    m_program = bgfx::createProgram(
        bgfx::createEmbeddedShader(s_embeddedShaders, renderer_type, "vs_cubes"),
        bgfx::createEmbeddedShader(s_embeddedShaders, renderer_type, "fs_cubes"),
        true /* destroy shaders when program is destroyed */
	);
}

BGFXWidget::~BGFXWidget()
{
    bgfx::destroy(m_ibh);
    bgfx::destroy(m_vbh);
    bgfx::destroy(m_program);
    bgfx::shutdown();
}

void BGFXWidget::setDefaultCamera()
{
    viewer_pos = bx::Vec3(0, 0, 10);
    viewer_target = bx::Vec3(0, 0, 0);
    viewer_up = bx::Vec3(0, 1, 0);
    rotation_radius = bx::length(sub(viewer_pos, viewer_target));
    viewer_previous_pos = viewer_pos;
    viewer_previous_target = viewer_target;
    viewer_previous_up = viewer_up;
    minimum_rotation_radius = 0.1;
    maximum_rotation_radius = 1000.0;
}

void BGFXWidget::draw()
{
    // Set view and projection matrix for view 0.
    {
        std::array<float, 16> view_matrix;
        bx::mtxLookAt(
            view_matrix.data(),
            viewer_pos,
            viewer_target,
            viewer_up
        );

        std::array<float, 16> projection_matrix;
        bx::mtxProj(
            projection_matrix.data(), 50.0f,
            realWidth() / realHeight(),
            1.0f,
            1024.0f,
            bgfx::getCaps()->homogeneousDepth
        );

        bgfx::setViewTransform(0, view_matrix.data(), projection_matrix.data());
        bgfx::setViewRect(0, 0, 0, realWidth(), realHeight());
    }

    // This dummy draw call is here to make sure that view 0 is cleared
    // if no other draw calls are submitted to view 0.
    bgfx::touch(0);

    // Set vertex and index buffer.
    bgfx::setVertexBuffer(0, m_vbh);
    bgfx::setIndexBuffer(m_ibh);
    // Set render states.
    bgfx::setState(BGFX_STATE_DEFAULT);
    // Submit primitive for rendering to view 0.
    bgfx::submit(0, m_program);

    // Advance to next frame. Rendering thread will be kicked to
    // process submitted rendering primitives.
    bgfx::frame();
}

void BGFXWidget::paintEvent(QPaintEvent*)
{
    if (is_bgfx_paint)
    {
        update();
        is_bgfx_paint = false;
        return;
    }

    draw();
    is_bgfx_paint = true;
}

void BGFXWidget::resizeEvent(QResizeEvent*)
{
    if (bgfx::getInternalData()->context != nullptr)
    {
        bgfx::reset(realWidth(), realHeight(), reset);
    }

    update();
}

void BGFXWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (left_mouse_pressed && right_mouse_pressed)
    {
        // Pan mode
        QPointF current_point = event->position();
        const double effective_rotation_radius = std::max(rotation_radius, 10.0);
        double delta_x = (current_point.x() - previous_point.x()) / width() * effective_rotation_radius;
        double delta_y = (current_point.y() - previous_point.y()) / height() * effective_rotation_radius;
        auto user_position = bx::sub(viewer_previous_pos, viewer_previous_target);
        auto right = bx::normalize(bx::cross(viewer_up, user_position));
        auto offset = bx::add(bx::mul(right, delta_x), bx::mul(viewer_up, delta_y));
        viewer_pos = bx::add(viewer_previous_pos, offset);
        viewer_target = bx::add(viewer_previous_target, offset);
        // Restore rotation orbit radius
        viewer_target = bx::add(viewer_pos, bx::mul(bx::normalize(bx::sub(viewer_target, viewer_pos)), rotation_radius));
    }
    else if (left_mouse_pressed)
    {
        // Rotation mode
        QPointF current_point = event->position();
        double delta_x = previous_point.x() - current_point.x();
        double delta_y = previous_point.y() - current_point.y();
        double x_rotation_angle = delta_x / width() * bx::kPi;
        double y_rotation_angle = delta_y / height() * bx::kPi;
        auto user_position = bx::sub(viewer_previous_pos, viewer_previous_target);
        auto rotation_x = bx::mul(viewer_previous_up, bx::rotateX(x_rotation_angle));
        bx::Vec3 temp_user_position = bx::mul(user_position, rotation_x);
        auto left = bx::normalize(bx::cross(temp_user_position, viewer_previous_up));
        auto rotation_y = bx::mul(left, bx::rotateY(y_rotation_angle));
        bx::Quaternion result_rotation = bx::mul(bx::fromEuler(rotation_x), bx::fromEuler(rotation_y));
        auto rotated_user_position = bx::mul(bx::normalize(mul(user_position, result_rotation)), rotation_radius);
        viewer_pos = bx::add(viewer_previous_target, rotated_user_position);
        viewer_up = bx::normalize(bx::mul(viewer_previous_up, result_rotation));
        // Restore up vector property: up vector must be orthogonal to direction vector
        auto new_left = bx::cross(rotated_user_position, viewer_up);
        viewer_up = bx::normalize(bx::cross(new_left, rotated_user_position));
    }
    else if (right_mouse_pressed)
    {
        // First person look mode
        QPointF current_point = event->position();
        double delta_x = current_point.x() - previous_point.x();
        double delta_y = current_point.y() - previous_point.y();
        double x_rotation_angle = delta_x / width() * bx::kPi;
        double y_rotation_angle = delta_y / height() * bx::kPi;
        auto view_direction = bx::sub(viewer_previous_target, viewer_previous_pos);
        auto rotation_x = bx::mul(viewer_previous_up, bx::rotateX(x_rotation_angle));
        bx::Vec3 temp_view_direction = bx::mul(view_direction, rotation_x);
        auto left = bx::normalize(bx::cross(viewer_previous_up, temp_view_direction));
        auto rotation_y = bx::mul(left, bx::rotateY(y_rotation_angle));
        bx::Quaternion result_rotation = bx::mul(bx::fromEuler(rotation_y), bx::fromEuler(rotation_x));
        auto rotated_view_direction = bx::mul(bx::normalize(bx::mul(view_direction, result_rotation)), rotation_radius);
        viewer_target = bx::add(viewer_previous_pos, rotated_view_direction);
        viewer_up = bx::normalize(bx::mul(viewer_previous_up, result_rotation));
        // Restore up vector property: up vector must be orthogonal to direction vector
        auto new_left = bx::cross(viewer_up, rotated_view_direction);
        viewer_up = bx::normalize(bx::cross(rotated_view_direction, new_left));
    }
    update();
}

void BGFXWidget::mousePressEvent(QMouseEvent *event)
{
    bool left_or_right = false;
    if (event->buttons() & Qt::LeftButton)
    {
        left_mouse_pressed = true;
        left_or_right = true;
    }
    if (event->buttons() & Qt::RightButton)
    {
        right_mouse_pressed = true;
        left_or_right = true;
    }
    if (left_or_right)
    {
        previous_point = event->position();
        viewer_previous_pos = viewer_pos;
        viewer_previous_target = viewer_target;
        viewer_previous_up = viewer_up;
    }
    update();
}

void BGFXWidget::mouseReleaseEvent(QMouseEvent *event)
{
    bool left_or_right = false;
    if (!(event->buttons() & Qt::LeftButton))
    {
        left_mouse_pressed = false;
        left_or_right = true;
    }
    if (!(event->buttons() & Qt::RightButton))
    {
        right_mouse_pressed = false;
        left_or_right = true;
    }
    if (left_or_right)
    {
        previous_point = event->position();
        viewer_previous_pos = viewer_pos;
        viewer_previous_target = viewer_target;
        viewer_previous_up = viewer_up;
    }
    update();
}

void BGFXWidget::wheelEvent(QWheelEvent *event)
{
    QPoint delta = event->angleDelta();
    rotation_radius += delta.y() / 1000.0f * rotation_radius;
    if (rotation_radius < minimum_rotation_radius)
    {
        rotation_radius = minimum_rotation_radius;
    }
    if (rotation_radius > maximum_rotation_radius)
    {
        rotation_radius = maximum_rotation_radius;
    }
    auto user_position = bx::sub(viewer_pos, viewer_target);
    auto new_user_position = bx::mul(bx::normalize(user_position), rotation_radius);
    viewer_pos = bx::add(viewer_target, new_user_position);
    update();
}
