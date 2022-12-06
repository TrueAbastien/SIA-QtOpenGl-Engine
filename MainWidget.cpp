
#include "MainWidget.h"

#include <QMouseEvent>

#include <cmath>


// ------------------------------------------------------------------------------------------------
MainWidget::MainWidget() :
  isCameraRotating(false)
{
  resetCamera();
}

// ------------------------------------------------------------------------------------------------
MainWidget::~MainWidget()
{
    // Make sure the context is current when deleting the texture
    // and the buffers.
    makeCurrent();
    delete scene;
    doneCurrent();
}

// ------------------------------------------------------------------------------------------------
void MainWidget::mousePressEvent(QMouseEvent *e)
{
  if (e->button() == Qt::LeftButton)
  {
    isCameraMoving = true;
    previousMousePos = QVector2D(e->localPos());
  }
  else if (e->button() == Qt::RightButton)
  {
    isCameraRotating = true;
    previousMousePos = QVector2D(e->localPos());
  }
}

// ------------------------------------------------------------------------------------------------
void MainWidget::mouseMoveEvent(QMouseEvent* e)
{
  QVector2D newPos = QVector2D(e->localPos());
  QVector2D delta = newPos - previousMousePos;

  if (isCameraMoving)
  {
    cameraPosition += (delta.x() * right + delta.y() * up) * 0.015f;

    previousMousePos = newPos;
    update();
  }
  else if (isCameraRotating)
  {
    cameraRotation += QVector2D(delta.x(), -delta.y()) * 0.01f;
    cameraRotation.setY(std::min(std::max(cameraRotation.y(), ROT_EPS), M_PI - ROT_EPS));

    previousMousePos = newPos;
    update();
  }
}

// ------------------------------------------------------------------------------------------------
void MainWidget::mouseReleaseEvent(QMouseEvent *e)
{
  if (e->button() == Qt::LeftButton)
  {
    isCameraMoving = false;
  }
  else if (e->button() == Qt::RightButton)
  {
    isCameraRotating = false;
  }
}

// ------------------------------------------------------------------------------------------------
void MainWidget::wheelEvent(QWheelEvent* event)
{
  float delta = event->angleDelta().y() * 0.005f;
  cameraDistance = std::min(std::max(cameraDistance - delta, 1.0f), 20.0f);

  update();
}

// ------------------------------------------------------------------------------------------------
void MainWidget::keyPressEvent(QKeyEvent* e)
{
  switch (e->key())
  {
    case Qt::Key_R:
      resetCamera();
      break;

    case Qt::Key_Escape:
      close();
      break;

    default: break;
  }
}

// ------------------------------------------------------------------------------------------------
void MainWidget::timerEvent(QTimerEvent *)
{
  //
}

// ------------------------------------------------------------------------------------------------
void MainWidget::initializeGL()
{
    initializeOpenGLFunctions();

    glClearColor(0, 0, 0, 1);

    // Enable depth buffer
    glEnable(GL_DEPTH_TEST);

    // Enable back face culling
    glEnable(GL_CULL_FACE);

    scene = new Scene;
    {
      // Cube 1
      {
        scene->addChildren(QSharedPointer<Cube>::create());
      }

      // Cube 2
      {
        auto cube = QSharedPointer<Cube>::create();
        cube->setLocalToParent(QVector3D(5.0, 0.0, 0.0));
        scene->addChildren(cube);
      }
    }
    scene->init();

    // Use QBasicTimer because its faster than QTimer
    timer.start(12, this);
}

// ------------------------------------------------------------------------------------------------
void MainWidget::resizeGL(int w, int h)
{
    // Calculate aspect ratio
    qreal aspect = qreal(w) / qreal(h ? h : 1);

    // Set near plane to 3.0, far plane to 7.0, field of view 45 degrees
    const qreal zNear = 1.0, zFar = 30.0, fov = 45.0;

    // Reset projection
    projection.setToIdentity();

    // Set perspective projection
    projection.perspective(fov, aspect, zNear, zFar);
}

// ------------------------------------------------------------------------------------------------
void MainWidget::paintGL()
{
    // Clear color and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Calculate model view transformation
    QMatrix4x4 view = updateView();

    // Draw cube geometry
    UpdateInfo infos;
    {
      infos.dt = 0.1f; // TODO
      infos.screenToParent = projection * view;
    }
    scene->update(infos);
}

// ------------------------------------------------------------------------------------------------
QMatrix4x4 MainWidget::updateView()
{
  forward = QVector3D(
    std::sin(cameraRotation.y()) * std::cos(cameraRotation.x()),
    std::sin(cameraRotation.y()) * std::sin(cameraRotation.x()),
    std::cos(cameraRotation.y())
  ).normalized();
  right = QVector3D::crossProduct(forward, QVector3D(0, 0, 1)).normalized();
  up = QVector3D::crossProduct(right, forward).normalized();

  QMatrix4x4 mat;
  mat.lookAt(cameraPosition + forward * cameraDistance, cameraPosition, up);
  return mat;
}

// ------------------------------------------------------------------------------------------------
void MainWidget::resetCamera()
{
  cameraDistance = 5.0f;
  cameraRotation = QVector2D(0, M_PI * .5f);
  cameraPosition = QVector3D(0, 0, 0);

  update();
}
