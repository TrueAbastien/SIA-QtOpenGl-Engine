
#include <QApplication>
#include <QLabel>
#include <QSurfaceFormat>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFile>

#ifndef QT_NO_OPENGL
#include "MainWidget.h"
#endif

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    QSurfaceFormat::setDefaultFormat(format);

    app.setApplicationName("siapp");
    app.setApplicationVersion("1.0");

#ifndef QT_NO_OPENGL

    MainWidget* widget = new MainWidget();
    widget->setFocusPolicy(Qt::StrongFocus);

    QMenuBar* menu = widget->makeMenu();
    menu->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    QWidget* controls = widget->makeControls();
    controls->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    QWidget* logger = widget->makeLogger();
    logger->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QHBoxLayout* hl = new QHBoxLayout;
    hl->addWidget(controls);
    hl->addWidget(widget);

    QVBoxLayout* vl = new QVBoxLayout;
    vl->addLayout(hl);
    vl->addWidget(logger);

    QVBoxLayout* ml = new QVBoxLayout;
    ml->addWidget(menu);
    ml->addLayout(vl);

    QWidget* win = new QWidget;
    win->setLayout(ml);
    win->setMinimumSize(600, 480);
    win->show();

    QFile file(":/styles/Genetive.qss");
    file.open(QFile::ReadOnly);
    QString styleSheet = QLatin1String(file.readAll());
    file.close();
    win->setStyleSheet(styleSheet);

#else
    QLabel note("OpenGL Support required");
    note.show();
#endif

    return app.exec();
}
