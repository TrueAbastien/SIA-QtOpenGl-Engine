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
    BodyBase.cpp \
    Box.cpp \
    ColoredRenderable.cpp \
    Component.cpp \
    Cube.cpp \
    FactoryFloor.cpp \
    FileReader.cpp \
    FileWriter.cpp \
    Floor.cpp \
    Frame.cpp \
    Joint.cpp \
    JointRenderer.cpp \
    MainWidget.cpp \
    MeshEditWindow.cpp \
    MeshRigRelation.cpp \
    MTAnimation.cpp \
    MTAnimationData.cpp \
    MTAnimatorPlug.cpp \
    MTBody.cpp \
    MTSetupWindow.cpp \
    MTSkinMesh.cpp \
    Renderable.cpp \
    Scene.cpp \
    SkinMesh.cpp \
    TexturedRenderable.cpp \
    WiredRenderable.cpp

HEADERS += \
    Animation.h \
    AnimationBase.h \
    AnimationController.h \
    AnimatorPlug.h \
    AnimatorPlugBase.h \
    AxisCorrector.h \
    BodyBase.h \
    Box.h \
    ColoredRenderable.h \
    Component.h \
    Cube.h \
    FactoryFloor.h \
    FileReader.h \
    FileWriter.h \
    Floor.h \
    Frame.h \
    Joint.h \
    JointRenderer.h \
    MainWidget.h \
    MeshEditWindow.h \
    MeshRigRelation.h \
    MTAnimation.h \
    MTAnimationData.h \
    MTAnimatorPlug.h \
    MTBody.h \
    MTSetupWindow.h \
    MTSkinMesh.h \
    Renderable.h \
    Scene.h \
    SkinMesh.h \
    SkinMeshBase.h \
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
