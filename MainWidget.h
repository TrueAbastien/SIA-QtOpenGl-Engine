
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
    QSharedPointer<T> createComponent(const QString& name, Args&&... args);
    template <typename T>
    QSharedPointer<T> createComponent(const QString& name, QSharedPointer<T> ptr);

    void internalLog(LogType type, const std::string& message);

public slots:

  void loadBVH();
  void loadOFF();
  void resetCamera();
  void recenterCamera();
  void startAnimation();
  void pauseAnimation();
  void stopAnimation();
  void clearScene();

signals:

  void animationTimeChanged(double value);

private:

  QMatrix4x4 updateView();

  QString getFileName(const QString& filePath) const;

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
inline QSharedPointer<T> MainWidget::createComponent(const QString& name, Args&& ...args)
{
  return createComponent(name, QSharedPointer<T>::create(args...));
}

// ------------------------------------------------------------------------------------------------
template<typename T>
inline QSharedPointer<T> MainWidget::createComponent(const QString& name, QSharedPointer<T> ptr)
{
  // Name
  ptr->setName(name);

  // Logger
  const auto logMethod = [&](LogType type, const std::string& message)
  {
    internalLog(type, message);
  };
  ptr->setLogger(logMethod);

  return ptr;
}
