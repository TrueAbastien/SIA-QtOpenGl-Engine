QT       += core gui widgets

TARGET = siapp
TEMPLATE = app

SOURCES += main.cpp

SOURCES += \
    ColoredRenderable.cpp \
    Component.cpp \
    Cube.cpp \
    MainWidget.cpp \
    Renderable.cpp \
    Scene.cpp \
    TexturedRenderable.cpp

HEADERS += \
    ColoredRenderable.h \
    Component.h \
    Cube.h \
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
