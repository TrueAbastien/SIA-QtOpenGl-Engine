
#pragma once

#include "Structs.h"

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QSharedPointer>
#include <QVector>

#include <functional>

class Component : protected QOpenGLFunctions
{
public:

    using Pointer = QSharedPointer<Component>;

    enum Notification
    {
      CHILD_ADDED
    };

    using MatrixConstruct = std::function<QMatrix4x4(QVector3D, QVector3D)>;

protected:

    Component();

public:

    virtual void init() = 0;
    virtual void update(UpdateInfo info) = 0;

public:

    void setName(const QString& name);
    QString name() const;

    void setLocalToParent(const QVector3D& pos = QVector3D(), const QVector3D& rot = QVector3D());
    QMatrix4x4 localToParent() const;
    QMatrix4x4 localToWorld() const;

    void setLocalPosition(const QVector3D& pos);
    QVector3D localPosition() const;

    void setLocalRotation(const QVector3D& rot);
    QVector3D localRotation() const;

    virtual void addChildren(const Pointer& child);
    QVector<Pointer> children() const;
    Pointer child(int index) const; 
    const Component* parent() const;

    void setLogger(LogMethod method);

    virtual void copy(const Pointer& pointer);

    void setMatrixConstruct(const MatrixConstruct& method);

protected:

    void setParent(Component* parent);

    void notifyParent(Notification notif);
    virtual void listenChild(Notification notif);

    void log(LogType type, const std::string& message);

    void updateTransform();

protected:

    QString m_name;

    QVector3D m_localPosition;
    QVector3D m_localRotation;
    QMatrix4x4 m_localToParent;

    QVector<Pointer> m_children;
    Component* m_parent;

    MatrixConstruct m_matrixMethod;

private:

  LogMethod logMethod;
};
