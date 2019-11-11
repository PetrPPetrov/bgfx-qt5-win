#pragma once

#include <QObject>
#include <QWidget>
#include <QResizeEvent>
#include "bgfx/bgfx.h"
#include "bx/bx.h"
#include "bx/math.h"

class BGFXWidget : public QWidget
{
    Q_OBJECT

public:
    BGFXWidget(QWidget *parent);
    ~BGFXWidget();
    void initializeBGFX(int width, int height, void* native_window_handle);

private:
    void setDefaultCamera();

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

public slots:
    void renderingUpdate();

private:
    uint32_t initial_width;
    uint32_t initial_height;
    uint32_t debug;
    uint32_t reset;
    bgfx::VertexBufferHandle m_vbh;
    bgfx::IndexBufferHandle m_ibh;
    bgfx::ProgramHandle m_program;

    bx::Vec3 viewer_pos;
    bx::Vec3 viewer_target;
    bx::Vec3 viewer_up;

    bx::Vec3 viewer_previous_pos;
    bx::Vec3 viewer_previous_target;
    bx::Vec3 viewer_previous_up;

    double minimum_rotation_radius = 0.1;
    double maximum_rotation_radius = 1000.0;
    double rotation_radius = 10.0;
    bool left_mouse_pressed = false;
    bool right_mouse_pressed = false;
    QPointF previous_point;
};
