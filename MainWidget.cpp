
#include "MainWidget.h"

#include "Cube.h"
#include "Box.h"
#include "Joint.h"
#include "JointRenderer.h"
#include "Floor.h"
#include "AnimatorPlug.h"
#include "FactoryFloor.h"
#include "FileReader.h"
#include "AxisCorrector.h"

#include <QMouseEvent>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QGroupBox>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QFileDialog>
#include <QInputDialog>

#include <cmath>


// ------------------------------------------------------------------------------------------------
MainWidget::MainWidget() :
  isCameraMoving(false),
  isCameraRotating(false),
  logger(nullptr)
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
QMenuBar* MainWidget::makeMenu()
{
  QMenuBar* root = new QMenuBar;

  // File menu
  {
    QMenu* menu = new QMenu("File");

    // Load BVH Action
    {
      QAction* action = new QAction("Load BVH");
      connect(action, &QAction::triggered, this, &MainWidget::loadBVH);
      menu->addAction(action);
    }

    // Load OFF Action
    {
      QAction* action = new QAction("Load OFF");
      connect(action, &QAction::triggered, this, &MainWidget::loadOFF);
      menu->addAction(action);
    }

    root->addMenu(menu);
  }

  // TODO

  return root;
}

// ------------------------------------------------------------------------------------------------
QWidget* MainWidget::makeControls()
{
  QGroupBox* root = new QGroupBox("Controls");
  QVBoxLayout* layout = new QVBoxLayout;

  // Scene
  {
    QGroupBox* box = new QGroupBox("Scene");
    QVBoxLayout* cameraVL = new QVBoxLayout;

    // Clear Button
    {
      QPushButton* button = new QPushButton("Clear");
      connect(button, &QPushButton::clicked, this, &MainWidget::clearScene);
      cameraVL->addWidget(button);
    }

    box->setLayout(cameraVL);
    layout->addWidget(box);
  }

  // Camera
  {
    QGroupBox* box = new QGroupBox("Camera");
    QVBoxLayout* cameraVL = new QVBoxLayout;

    // Reset Button
    {
      QPushButton* button = new QPushButton("Reset");
      connect(button, &QPushButton::clicked, this, &MainWidget::resetCamera);
      cameraVL->addWidget(button);
    }

    // Recenter Button
    {
      QPushButton* button = new QPushButton("Recenter");
      connect(button, &QPushButton::clicked, this, &MainWidget::recenterCamera);
      cameraVL->addWidget(button);
    }

    box->setLayout(cameraVL);
    layout->addWidget(box);
  }

  // Animation
  {
    QGroupBox* box = new QGroupBox("Animation");
    QVBoxLayout* animVL = new QVBoxLayout;

    // Start Button
    {
      QPushButton* button = new QPushButton("Start");
      connect(button, &QPushButton::clicked, this, &MainWidget::startAnimation);
      animVL->addWidget(button);
    }

    // Pause Button
    {
      QPushButton* button = new QPushButton("Pause");
      connect(button, &QPushButton::clicked, this, &MainWidget::pauseAnimation);
      animVL->addWidget(button);
    }

    // Stop Button
    {
      QPushButton* button = new QPushButton("Stop");
      connect(button, &QPushButton::clicked, this, &MainWidget::stopAnimation);
      animVL->addWidget(button);
    }

    // Time Label
    {
      QHBoxLayout* timeHL = new QHBoxLayout;

      // Label
      {
        QLabel* label = new QLabel("Time:");
        timeHL->addWidget(label);
      }

      // Value
      {
        QDoubleSpinBox* spinBox = new QDoubleSpinBox;
        spinBox->setReadOnly(true);
        spinBox->setButtonSymbols(QAbstractSpinBox::NoButtons);
        connect(this, &MainWidget::animationTimeChanged, spinBox, &QDoubleSpinBox::setValue);
        timeHL->addWidget(spinBox);
      }

      animVL->addLayout(timeHL);
    }

    box->setLayout(animVL);
    layout->addWidget(box);
  }

  layout->setAlignment(Qt::AlignTop);
  root->setLayout(layout);
  root->setFixedWidth(200);
  return root;
}

// ------------------------------------------------------------------------------------------------
QWidget* MainWidget::makeLogger()
{
  if (logger != nullptr) delete logger;

  QGroupBox* root = new QGroupBox("Logger");
  QVBoxLayout* layout = new QVBoxLayout;

  logger = new QTextEdit;
  logger->setReadOnly(true);
  layout->addWidget(logger);

  root->setLayout(layout);
  root->setFixedHeight(200);
  root->setFocusPolicy(Qt::NoFocus);
  return root;
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
    cameraPosition += (delta.x() * right + delta.y() * up) * cameraDistance * 6e-4f;

    previousMousePos = newPos;
    update();
  }
  else if (isCameraRotating)
  {
    cameraRotation += QVector2D(delta.x(), -delta.y()) * 1e-2f;
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
  cameraDistance = std::min(std::max(cameraDistance - delta, 1.0f), 100.0f);

  update();
}

// ------------------------------------------------------------------------------------------------
void MainWidget::keyPressEvent(QKeyEvent* e)
{
  switch (e->key())
  {
    // Close Application
    case Qt::Key_Escape:
      close();
      break;

    default: break;
  }
}

// ------------------------------------------------------------------------------------------------
void MainWidget::timerEvent(QTimerEvent *)
{
  update();

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
    clearScene();

    // Use QBasicTimer because its faster than QTimer
    timer.start(12, this);
}

// ------------------------------------------------------------------------------------------------
void MainWidget::resizeGL(int w, int h)
{
    // Calculate aspect ratio
    qreal aspect = qreal(w) / qreal(h ? h : 1);

    // Set near plane to 3.0, far plane to 7.0, field of view 45 degrees
    const qreal zNear = 1.0, zFar = 50.0, fov = 45.0;

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
    float animTime = animController.time();
    UpdateInfo infos;
    {
      infos.dt = 0.1f; // TODO
      infos.animationTime = animTime,
      infos.screenToParent = projection * view;
    }
    scene->update(infos);

    emit animationTimeChanged(animTime);
}

// ------------------------------------------------------------------------------------------------
void MainWidget::internalLog(LogType type, const std::string& message)
{
  if (logger == nullptr)
  {
    return;
  }

  bool flag = true;
  std::string colorCode;

  // Color Pick
  switch (type)
  {
#ifdef _DEBUG
    case DEBUG:     colorCode = "#6224c7"; break;
#endif
    case INFO:      colorCode = "#2b7ce0"; break;
    case WARNING:   colorCode = "#e6a73c"; break;
    case ERROR_:    colorCode = "#c7341a"; break;
    case CRITICAL:  colorCode = "#591515"; break;

    default: flag = false; break;
  }
  if (!flag) return;

  // Display Message
  QTextCursor cursor = logger->textCursor();
  std::string content = "<font color=\"" + colorCode + "\">" + message + "</font><br>";
  logger->insertHtml(QString::fromStdString(content));
  cursor.movePosition(QTextCursor::End);
  logger->setTextCursor(cursor);
}

// ------------------------------------------------------------------------------------------------
void MainWidget::loadBVH()
{
  QString fileName = QFileDialog::getOpenFileName(this, tr("Open BVH"), "", tr("BVH Files (*.bvh)"));
  if (fileName.isEmpty())
  {
    return;
  }

  bool ok;
  double scale = QInputDialog::getDouble(this, "BVH Infos", "Scale Percent", 100.0f, 0.0f, 100.0f, 2, &ok);
  if (!ok)
  {
    return;
  }

  FileReader::BVHParameters params;
  {
    params.scale = 1e-2f * scale;
  }

  auto result = FileReader::readBVH(fileName, params);
  if (result == nullptr)
  {
    return;
  }

  auto parent = createComponent<AxisCorrector>(AxisCorrector::Mode::Y_to_Z);
  parent->addChildren(result);

  scene->addChildren(parent);
}

// ------------------------------------------------------------------------------------------------
void MainWidget::loadOFF()
{
  QString fileName = QFileDialog::getOpenFileName(this, tr("Open OFF"), "", tr("OFF Files (*.off)"));
  if (fileName.isEmpty())
  {
    return;
  }

  bool ok;
  double scale = QInputDialog::getDouble(this, "OFF Infos", "Scale Percent", 100.0f, 0.0f, 100.0f, 2, &ok);
  if (!ok)
  {
    return;
  }

  FileReader::OFFParameters params;
  {
    params.scale = 1e-2f * scale;
  }

  auto result = FileReader::readOFF(fileName, params);
  if (result == nullptr)
  {
    return;
  }

  auto parent = createComponent<AxisCorrector>(AxisCorrector::Mode::Y_to_Z);
  parent->addChildren(result);

  scene->addChildren(parent);
}

// ------------------------------------------------------------------------------------------------
void MainWidget::resetCamera()
{
  cameraDistance = 5.0f;
  cameraRotation = QVector2D(0, M_PI * .5f);
  cameraPosition = QVector3D(0, 0, 0);

  update();
}

// ------------------------------------------------------------------------------------------------
void MainWidget::recenterCamera()
{
  cameraPosition = QVector3D(0, 0, 0);

  update();
}

// ------------------------------------------------------------------------------------------------
void MainWidget::startAnimation()
{
  animController.start();
}

// ------------------------------------------------------------------------------------------------
void MainWidget::pauseAnimation()
{
  animController.pause();
}

// ------------------------------------------------------------------------------------------------
void MainWidget::stopAnimation()
{
  animController.stop();
}

// ------------------------------------------------------------------------------------------------
void MainWidget::clearScene()
{
  scene->clear();

  // Basic Scene
  {
    // Floor
    {
      scene->addChildren(createComponent<FactoryFloor>());
    }
  }

  update();
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
