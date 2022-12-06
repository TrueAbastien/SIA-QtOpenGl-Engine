
#pragma once

#include "Cube.h"
#include "Scene.h"

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QMatrix4x4>
#include <QQuaternion>
#include <QVector2D>
#include <QBasicTimer>

class MainWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
  Q_OBJECT;

public:

    using QOpenGLWidget::QOpenGLWidget;

    MainWidget();
    ~MainWidget();

protected:

    void mousePressEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void wheelEvent(QWheelEvent* e) override;
    void keyPressEvent(QKeyEvent* e) override;
    void timerEvent(QTimerEvent *e) override;

    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

private:

  QMatrix4x4 updateView();

  void resetCamera();

private:

    QBasicTimer timer;
    Scene* scene;

    QMatrix4x4 projection;

    // Camera Controls
    QVector2D previousMousePos;
    bool isCameraRotating;
    bool isCameraMoving;
    QVector2D cameraRotation;
    float cameraDistance;
    QVector3D cameraPosition;
    QVector3D forward, right, up;
};
