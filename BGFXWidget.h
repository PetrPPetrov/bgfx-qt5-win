#pragma once

#include <QWidget>

#include <bgfx/bgfx.h>
#include <bx/bx.h>
#include <bx/math.h>

class BGFXWidget : public QWidget
{
    Q_OBJECT

public:
    BGFXWidget(QWidget *parent = nullptr);
    ~BGFXWidget();

    void setRendererType(bgfx::RendererType::Enum renderer_type) { this->renderer_type = renderer_type; }

private:
    void setDefaultCamera();
    void draw();

    qreal realWidth() const { return width() * devicePixelRatio(); }
    qreal realHeight() const { return height() * devicePixelRatio(); }

protected:
    void showEvent(QShowEvent* event) override;
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    bgfx::RendererType::Enum renderer_type = bgfx::RendererType::OpenGL;

    uint32_t debug;
    uint32_t reset;
    bgfx::VertexBufferHandle m_vbh;
    bgfx::IndexBufferHandle m_ibh;
    bgfx::ProgramHandle m_program;

    bx::Vec3 viewer_pos{ bx::InitNone };
    bx::Vec3 viewer_target{ bx::InitNone };
    bx::Vec3 viewer_up{ bx::InitNone };

    bx::Vec3 viewer_previous_pos{ bx::InitNone };
    bx::Vec3 viewer_previous_target{ bx::InitNone };
    bx::Vec3 viewer_previous_up{ bx::InitNone };

    double minimum_rotation_radius = 0.1;
    double maximum_rotation_radius = 1000.0;
    double rotation_radius = 10.0;
    bool left_mouse_pressed = false;
    bool right_mouse_pressed = false;
    QPointF previous_point;

    bool is_bgfx_paint = false;
};
