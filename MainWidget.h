
#pragma once

#include "Scene.h"
#include "AnimationController.h"
#include "MeshEditWindow.h"
#include "MTSetupWindow.h"

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QMatrix4x4>
#include <QQuaternion>
#include <QVector2D>
#include <QBasicTimer>
#include <QMenuBar>
#include <QTextEdit>
#include <QDropEvent>


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

    void dragEnterEvent(QDragEnterEvent* e) override;
    void dropEvent(QDropEvent* e) override;

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
  void saveMTBody();

  void openMeshEdit();
  void openMTSetup();

  void makeSkin();
  void resetCamera();
  void recenterCamera();
  void startAnimation();
  void pauseAnimation();
  void stopAnimation();
  void clearScene();

  void changeAmbient(int value);
  void changeDiffuse(int value);
  void changeSpecular(int value);
  void changeShininess(int value);

  void clearLogger();
  void resizeLoggerText(int value);

signals:

  void animationTimeChanged(double value);

private:

  QMatrix4x4 updateView();

  QString getFileName(const QString& filePath) const;

  void openFile(const QString& filePath);
  void openBVH(const QString& filePath, const FileReader::BVHParameters& params);
  void openOFF(const QString& filePath, const FileReader::OFFParameters& params);
  //

  template <typename T>
  using ComponentPredicate = std::function<bool(const QSharedPointer<T>&)>;
  template <typename T>
  QVector<QSharedPointer<T>> find_if(Component* parent = nullptr,
                                     ComponentPredicate<T> pred =
                                     [](const QSharedPointer<T>&) -> bool
                                     {
                                       return true;
                                     });

private:

    QBasicTimer timer;
    Scene* scene;
    AnimationController animController;
    GlobalMaterial material;
    float previousTime;

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

    // Tool Sub-Windows
    MeshEditWindow* meshEditWindow;
    MTSetupWindow* setupMTWindow;
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

// ------------------------------------------------------------------------------------------------
template<typename T>
inline QVector<QSharedPointer<T>> MainWidget::find_if(Component* parent, ComponentPredicate<T> pred)
{
  using CompVec = QVector<QSharedPointer<T>>;

  CompVec result(0);

  QVector<Component*> next{parent == nullptr ? scene : parent}, curr;
  int iter = 0;

  // Iterative Search
  while (!next.isEmpty() && iter < 100)
  {
    curr = std::move(next);
    next.clear();

    for (const auto& item : curr)
    {
      for (const auto& child : item->children())
      {
        auto childCast = child.dynamicCast<T>();
        if (!childCast.isNull() && pred(childCast))
        {
          result.push_back(childCast);
        }

        next.push_back(child.get());
      }
    }
  }

  return result;
}
