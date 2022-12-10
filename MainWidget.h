
#pragma once

#include "Scene.h"
#include "AnimationController.h"

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QMatrix4x4>
#include <QQuaternion>
#include <QVector2D>
#include <QBasicTimer>
#include <QMenuBar>
#include <QTextEdit>


class MainWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
  Q_OBJECT;

public:

    using QOpenGLWidget::QOpenGLWidget;

    MainWidget();
    ~MainWidget();

public:

  QMenuBar* makeMenu();
  QWidget* makeControls();
  QWidget* makeLogger();

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

    template <typename T, typename ...Args>
    QSharedPointer<T> createComponent(Args&&... args);

    void internalLog(LogType type, const std::string& message);

public slots:

  void loadBVH();

private:

  QMatrix4x4 updateView();

  void resetCamera();

private:

    QBasicTimer timer;
    Scene* scene;
    AnimationController animController;

    QMatrix4x4 projection;
    QTextEdit* logger;

    // Camera Controls
    QVector2D previousMousePos;
    bool isCameraRotating;
    bool isCameraMoving;
    QVector2D cameraRotation;
    float cameraDistance;
    QVector3D cameraPosition;
    QVector3D forward, right, up;
};

// ------------------------------------------------------------------------------------------------
template<typename T, typename ...Args>
inline QSharedPointer<T> MainWidget::createComponent(Args&& ...args)
{
  auto result = QSharedPointer<T>::create(args...);

  // Logger
  const auto logMethod = [&](LogType type, const std::string& message)
  {
    internalLog(type, message);
  };
  result->setLogger(logMethod);

  return result;
}
