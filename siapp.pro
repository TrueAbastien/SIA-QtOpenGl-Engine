QT       += core gui widgets

TARGET = siapp
TEMPLATE = app

win32:RC_ICONS += icon.ico

SOURCES += main.cpp

SOURCES += \
    Animation.cpp \
    AnimationController.cpp \
    AnimatorPlug.cpp \
    AxisCorrector.cpp \
    Box.cpp \
    ColoredRenderable.cpp \
    Component.cpp \
    Cube.cpp \
    FactoryFloor.cpp \
    FileReader.cpp \
    FileWriter.cpp \
    Floor.cpp \
    Joint.cpp \
    JointRenderer.cpp \
    MainWidget.cpp \
    MeshEditWindow.cpp \
    MeshRigRelation.cpp \
    Renderable.cpp \
    Scene.cpp \
    SkinMesh.cpp \
    TexturedRenderable.cpp \
    WiredRenderable.cpp

HEADERS += \
    Animation.h \
    AnimationController.h \
    AnimatorPlug.h \
    AxisCorrector.h \
    Box.h \
    ColoredRenderable.h \
    Component.h \
    Cube.h \
    FactoryFloor.h \
    FileReader.h \
    FileWriter.h \
    Floor.h \
    Joint.h \
    JointRenderer.h \
    MainWidget.h \
    MeshEditWindow.h \
    MeshRigRelation.h \
    Renderable.h \
    Scene.h \
    SkinMesh.h \
    Structs.h \
    TexturedRenderable.h \
    WiredRenderable.h

RESOURCES += \
    shaders.qrc \
    styles.qrc \
    textures.qrc

# install
# target.path = $$[QT_INSTALL_EXAMPLES]/opengl/cube
# INSTALLS += target
