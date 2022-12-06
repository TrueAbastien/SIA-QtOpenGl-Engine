QT       += core gui widgets

TARGET = siapp
TEMPLATE = app

SOURCES += main.cpp

SOURCES += \
    Box.cpp \
    ColoredRenderable.cpp \
    Component.cpp \
    Cube.cpp \
    Floor.cpp \
    Joint.cpp \
    MainWidget.cpp \
    Renderable.cpp \
    Scene.cpp \
    TexturedRenderable.cpp

HEADERS += \
    Box.h \
    ColoredRenderable.h \
    Component.h \
    Cube.h \
    Floor.h \
    Joint.h \
    MainWidget.h \
    Renderable.h \
    Scene.h \
    TexturedRenderable.h

RESOURCES += \
    shaders.qrc \
    textures.qrc

# install
# target.path = $$[QT_INSTALL_EXAMPLES]/opengl/cube
# INSTALLS += target
