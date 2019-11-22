#include <cassert>
#include <fstream>
#include "BGFXWidget.h"

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

inline const bgfx::Memory* loadMem(const std::string& filename)
{
    std::ifstream in(filename.c_str(), std::ios_base::binary);
    in.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    in.seekg(0, std::ifstream::end);
    const uint32_t file_size(static_cast<uint32_t>(in.tellg()));
    in.seekg(0, std::ifstream::beg);
    const bgfx::Memory* mem = bgfx::alloc(file_size + 1);
    if (mem && file_size > 0)
    {
        in.read(reinterpret_cast<char*>(mem->data), file_size);
        mem->data[mem->size - 1] = '\0';
    }
    return mem;
}

inline bgfx::ShaderHandle loadShader(const std::string& filename)
{
    std::string shader_path;

    switch (bgfx::getRendererType())
    {
    default:
    case bgfx::RendererType::Noop:
        assert(false);
    case bgfx::RendererType::Direct3D9:  shader_path = "shaders/dx9/";   break;
    case bgfx::RendererType::Direct3D11:
    case bgfx::RendererType::Direct3D12: shader_path = "shaders/dx11/";  break;
    case bgfx::RendererType::Gnm:        shader_path = "shaders/pssl/";  break;
    case bgfx::RendererType::Metal:      shader_path = "shaders/metal/"; break;
    case bgfx::RendererType::Nvn:        shader_path = "shaders/nvn/";   break;
    case bgfx::RendererType::OpenGL:     shader_path = "shaders/glsl/";  break;
    case bgfx::RendererType::OpenGLES:   shader_path = "shaders/essl/";  break;
    case bgfx::RendererType::Vulkan:     shader_path = "shaders/spirv/"; break;
    }

    std::string file_path = shader_path + filename + ".bin";

    bgfx::ShaderHandle handle = bgfx::createShader(loadMem(file_path));
    bgfx::setName(handle, filename.c_str());

    return handle;
}

inline bgfx::ProgramHandle loadProgram(const std::string& vs_name, const std::string& fs_name)
{
    bgfx::ShaderHandle vsh = loadShader(vs_name);
    bgfx::ShaderHandle fsh = BGFX_INVALID_HANDLE;
    if (!fs_name.empty())
    {
        fsh = loadShader(fs_name);
    }

    return bgfx::createProgram(vsh, fsh);
}

BGFXWidget::BGFXWidget(QWidget *parent) : QOpenGLWidget(parent)
{
    setDefaultCamera();
}

void BGFXWidget::initializeBGFX(int width, int height, void* native_window_handle)
{
    initial_width = width;
    initial_height = height;
    debug = BGFX_DEBUG_NONE;
    reset = BGFX_RESET_NONE;

    bgfx::Init init;
    init.type = bgfx::RendererType::Direct3D9; // Or OpenGL or Direct3D11
    init.vendorId = BGFX_PCI_ID_NONE;
    init.resolution.width = width;
    init.resolution.height = height;
    init.resolution.reset = reset;
    init.platformData.nwh = native_window_handle;
    bgfx::init(init);

    bgfx::setDebug(debug);
    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x303030ff, 1.0f, 0);

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
    m_program = loadProgram("vs_cubes", "fs_cubes");
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

void BGFXWidget::paintEvent(QPaintEvent* event)
{
    float view_matrix[16];
    bx::mtxLookAt(
        view_matrix,
        viewer_pos,
        viewer_target,
        viewer_up
    );

    float projection_matrix[16];
    bx::mtxProj(
        projection_matrix, 50.0f,
        static_cast<float>(width()) / static_cast<float>(height()),
        1.0f,
        1024.0f,
        bgfx::getCaps()->homogeneousDepth
    );
    bgfx::setViewTransform(0, view_matrix, projection_matrix);
    bgfx::setViewRect(0, 0, 0, width(), height());

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

void BGFXWidget::resizeEvent(QResizeEvent* event)
{
    // TODO:
}

void BGFXWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (left_mouse_pressed && right_mouse_pressed)
    {
        // Pan mode
        QPointF current_point = event->localPos();
        const double effective_rotation_radius = std::max(rotation_radius, 10.0);
        double delta_x = (current_point.x() - previous_point.x()) / width() * effective_rotation_radius;
        double delta_y = (current_point.y() - previous_point.y()) / height() * effective_rotation_radius;
        auto user_position = sub(viewer_previous_pos, viewer_previous_target);
        auto right = normalize(cross(viewer_up, user_position));
        auto offset = add(mul(right, delta_x), mul(viewer_up, delta_y));
        viewer_pos = add(viewer_previous_pos, offset);
        viewer_target = add(viewer_previous_target, offset);
        // Restore rotation orbit radius
        viewer_target = add(viewer_pos, mul(normalize(sub(viewer_target, viewer_pos)), rotation_radius));
        update();
    }
    else if (left_mouse_pressed)
    {
        // Rotation mode
        QPointF current_point = event->localPos();
        double delta_x = previous_point.x() - current_point.x();
        double delta_y = previous_point.y() - current_point.y();
        double x_rotation_angle = delta_x / width() * bx::kPi;
        double y_rotation_angle = delta_y / height() * bx::kPi;
        auto user_position = sub(viewer_previous_pos, viewer_previous_target);
        auto rotation_x = bx::rotateAxis(viewer_previous_up, x_rotation_angle);
        bx::Vec3 temp_user_position = mul(user_position, rotation_x);
        auto left = normalize(cross(temp_user_position, viewer_previous_up));
        auto rotation_y = bx::rotateAxis(left, y_rotation_angle);
        bx::Quaternion result_rotation = mul(rotation_x, rotation_y);
        auto rotated_user_position = mul(normalize(mul(user_position, result_rotation)), rotation_radius);
        viewer_pos = add(viewer_previous_target, rotated_user_position);
        viewer_up = normalize(mul(viewer_previous_up, result_rotation));
        // Restore up vector property: up vector must be orthogonal to direction vector
        auto new_left = cross(rotated_user_position, viewer_up);
        viewer_up = normalize(cross(new_left, rotated_user_position));
        update();
    }
    else if (right_mouse_pressed)
    {
        // First person look mode
        QPointF current_point = event->localPos();
        double delta_x = current_point.x() - previous_point.x();
        double delta_y = current_point.y() - previous_point.y();
        double x_rotation_angle = delta_x / width() * bx::kPi;
        double y_rotation_angle = delta_y / height() * bx::kPi;
        auto view_direction = sub(viewer_previous_target, viewer_previous_pos);
        auto rotation_x = bx::rotateAxis(viewer_previous_up, x_rotation_angle);
        bx::Vec3 temp_view_direction = mul(view_direction, rotation_x);
        auto left = normalize(cross(viewer_previous_up, temp_view_direction));
        auto rotation_y = bx::rotateAxis(left, y_rotation_angle);
        bx::Quaternion result_rotation = mul(rotation_y, rotation_x);
        auto rotated_view_direction = mul(normalize(mul(view_direction, result_rotation)), rotation_radius);
        viewer_target = add(viewer_previous_pos, rotated_view_direction);
        viewer_up = normalize(mul(viewer_previous_up, result_rotation));
        // Restore up vector property: up vector must be orthogonal to direction vector
        auto new_left = cross(viewer_up, rotated_view_direction);
        viewer_up = normalize(cross(rotated_view_direction, new_left));
        update();
    }
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
        previous_point = event->localPos();
        viewer_previous_pos = viewer_pos;
        viewer_previous_target = viewer_target;
        viewer_previous_up = viewer_up;
    }
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
        previous_point = event->localPos();
        viewer_previous_pos = viewer_pos;
        viewer_previous_target = viewer_target;
        viewer_previous_up = viewer_up;
    }
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
    auto user_position = sub(viewer_pos, viewer_target);
    auto new_user_position = mul(normalize(user_position), rotation_radius);
    viewer_pos = add(viewer_target, new_user_position);
    update();
}
