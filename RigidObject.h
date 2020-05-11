#ifndef RIGIDOBJECT_H
#define RIGIDOBJECT_H

#include <QPolygon>
#include <QColor>
#include <QVector2D>

class RigidObject {
public:
    RigidObject() = default;
    RigidObject(int width, int height);

    const QPolygon& getVertices() const;
    const QColor& getColor() const;

    void move(QVector2D& positionDelta);
    void setPosition(QVector2D& position);
    const QVector2D& getPosition() const;

    void linearAccelerate(QVector2D& linearVelocityDelta);
    void setLinearVelocity(QVector2D& linearVelocity);
    const QVector2D& getLinearVelocity() const;

    float getMass() const;

    void setPinned(bool pinned);
    bool isPinned() const;

    void setCollides(bool collides);
    bool isCollides() const;

    bool collidesWith(RigidObject* object);

private:
    bool collidesQuick(RigidObject* object) const;
    bool collidesExact(RigidObject* object) const;

    QPolygon vertices;
    QColor color;

    QVector2D position;
    QVector2D linearVelocity;
    float mass = 1.0f;

    bool pinned = false;
    bool collides = false;
};

#endif // RIGIDOBJECT_H
