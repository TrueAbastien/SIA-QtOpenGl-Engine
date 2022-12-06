
#pragma once

#include "Structs.h"

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QSharedPointer>
#include <QVector>

class Component : protected QOpenGLFunctions
{
public:

    using Pointer = QSharedPointer<Component>;

    enum Notification
    {
      CHILD_ADDED
    };

protected:

    Component();

public:

    virtual void init() = 0;
    virtual void update(UpdateInfo info) = 0;

public:

    void setLocalToParent(const QVector3D& pos = QVector3D(), const QVector3D& rot = QVector3D());
    void setLocalToParent(const QMatrix4x4& localToParent);
    QMatrix4x4 localToParent() const;
    QMatrix4x4 localToWorld() const;

    virtual void addChildren(const Pointer& child);
    QVector<Pointer> children() const;
    Pointer child(int index) const; 

protected:

    void setParent(Component* parent);

    void notifyParent(Notification notif);
    virtual void listenChild(Notification notif);

protected:

    QMatrix4x4 m_localToParent;
    QVector<Pointer> m_children;
    Component* m_parent;
};
