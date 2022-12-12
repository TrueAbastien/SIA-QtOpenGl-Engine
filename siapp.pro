QT       += core gui widgets

TARGET = siapp
TEMPLATE = app

win32:RC_ICONS += icon.ico

SOURCES += main.cpp

SOURCES += \
    Animation.cpp \
    AnimationController.cpp \
    AnimatorPlug.cpp \
    Box.cpp \
    ColoredRenderable.cpp \
    Component.cpp \
    Cube.cpp \
    FactoryFloor.cpp \
    FileReader.cpp \
    Floor.cpp \
    Joint.cpp \
    JointRenderer.cpp \
    MainWidget.cpp \
    Renderable.cpp \
    Scene.cpp \
    TexturedRenderable.cpp

HEADERS += \
    Animation.h \
    AnimationController.h \
    AnimatorPlug.h \
    Box.h \
    ColoredRenderable.h \
    Component.h \
    Cube.h \
    FactoryFloor.h \
    FileReader.h \
    Floor.h \
    Joint.h \
    JointRenderer.h \
    MainWidget.h \
    Renderable.h \
    Scene.h \
    Structs.h \
    TexturedRenderable.h

RESOURCES += \
    shaders.qrc \
    styles.qrc \
    textures.qrc

# install
# target.path = $$[QT_INSTALL_EXAMPLES]/opengl/cube
# INSTALLS += target
