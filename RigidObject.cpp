#include "RigidObject.h"

#include <QTime>
#include <QPoint>
#include <random>

RigidObject::RigidObject(int width, int height) {
    int x = width / 2;
    int y = height / 2;
    this->vertices << QPoint(x, y) << QPoint(x, -y) << QPoint(-x, -y) << QPoint(-x, y);

    static std::mt19937 generator(QTime::currentTime().msec());
    std::uniform_int_distribution<int> randomColor(128, 255);

    this->color.setRed(randomColor(generator));
    this->color.setGreen(randomColor(generator));
    this->color.setBlue(randomColor(generator));
}

const QPolygon& RigidObject::getVertices() const {
    return this->vertices;
}

const QColor& RigidObject::getColor() const {
    return this->color;
}

void RigidObject::move(QVector2D& positionDelta) {
    this->position += positionDelta;
}

void RigidObject::setPosition(QVector2D& position) {
    this->position = position;
}

const QVector2D& RigidObject::getPosition() const {
    return this->position;
}

void RigidObject::linearAccelerate(QVector2D& linearVelocityDelta) {
    this->linearVelocity += linearVelocityDelta;
}

void RigidObject::setLinearVelocity(QVector2D& linearVelocity) {
    this->linearVelocity = linearVelocity;
}

const QVector2D& RigidObject::getLinearVelocity() const {
    return this->linearVelocity;
}

float RigidObject::getMass() const {
    return this->mass;
}

void RigidObject::setPinned(bool pinned) {
    this->pinned = pinned;
    if (this->pinned) {
        this->linearVelocity = QVector2D();
    }
}

bool RigidObject::isPinned() const {
    return this->pinned;
}

void RigidObject::setCollides(bool collides) {
    this->collides = collides;
}

bool RigidObject::isCollides() const {
    return this->collides;
}

bool RigidObject::collidesWith(RigidObject* object) {
    return this->collidesQuick(object) && this->collidesExact(object);
}

bool RigidObject::collidesQuick(RigidObject* object) const {
    QRect boundingBox(this->vertices.boundingRect());
    QRect objectBoundingBox(object->getVertices().boundingRect());

    // Assume object origin is in the middle of the object
    QVector2D boundingCircleRadius(boundingBox.topLeft() - boundingBox.center());
    QVector2D objectBoundingCircleRadius(objectBoundingBox.topLeft() - objectBoundingBox.center());

    float boundingCircleSquareRadius = boundingCircleRadius.lengthSquared();
    float objectBoundingCircleSquareRadius = objectBoundingCircleRadius.lengthSquared();
    float collisionSquareDistance =
        boundingCircleSquareRadius + objectBoundingCircleSquareRadius +
        2 * sqrtf(boundingCircleSquareRadius * objectBoundingCircleSquareRadius);

    QVector2D objectsDistance(this->position - object->getPosition());
    float objectsSquareDistance = objectsDistance.lengthSquared();

    return objectsSquareDistance < collisionSquareDistance;
}

bool RigidObject::collidesExact(RigidObject* object) const {
    return true;  // TODO
}
