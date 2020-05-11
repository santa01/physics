#include "Physics.h"

#include <utility>

BoundingPoint::BoundingPoint(BoxBound bound, RigidObject* object):
        bound(bound),
        object(object) {
}

int BoundingPoint::getCoordinate(SweepAxis sweepAxis) const {
    bool sweepAxisX = (sweepAxis == SweepAxis::X);

    QRect boundingBox(this->object->getVertices().boundingRect());
    boundingBox.translate(this->object->getPosition().toPoint());

    if (this->bound == BoxBound::Begin) {
        return sweepAxisX ? boundingBox.left() : boundingBox.bottom();
    } else {
        return sweepAxisX ? boundingBox.right() : boundingBox.top();
    }
}

BoxBound BoundingPoint::getBound() const {
    return this->bound;
}

RigidObject* BoundingPoint::getObject() const {
    return this->object;
}

Physics::Physics(std::vector<RigidObject>& objects, std::mutex& objectsMutex):
        QObject(nullptr),
        objects(objects),
        objectsMutex(objectsMutex) {
}

void Physics::sweepAndPrune(SweepAxis sweepAxis, std::vector<CollisionGroup>& activeGroups) {
    if (this->boundingPoints.empty()) {
        this->sweepBoundingPoints();
    }

    this->sortBoundingPoints(sweepAxis);
    this->pruneBoundingPoints(activeGroups);
}

void Physics::sweepBoundingPoints() {
    for (RigidObject& object: this->objects) {
        this->boundingPoints.emplace_back(BoxBound::Begin, &object);
        this->boundingPoints.emplace_back(BoxBound::End, &object);
    }
}

void Physics::sortBoundingPoints(SweepAxis sweepAxis) {
    int pointsTotal = this->boundingPoints.size();

    for (int i = 1; i < pointsTotal; i++) {
        for (int j = i; j > 0; j--) {
            BoundingPoint& currentPoint = this->boundingPoints[j];
            BoundingPoint& previousPoint = this->boundingPoints[j - 1];

            if (currentPoint.getCoordinate(sweepAxis) < previousPoint.getCoordinate(sweepAxis)) {
                std::swap(currentPoint, previousPoint);
            }
        }
    }
}

void Physics::pruneBoundingPoints(std::vector<CollisionGroup>& activeGroups) {
    int groupObjects = 0;
    CollisionGroup* currentGroup = nullptr;

    for (BoundingPoint& point: this->boundingPoints) {
        if (currentGroup == nullptr) {
            activeGroups.resize(activeGroups.size() + 1);
            currentGroup = &activeGroups.back();
        }

        if (point.getBound() == BoxBound::Begin) {
            groupObjects++;
        } else {
            currentGroup->insert(point.getObject());
            groupObjects--;
        }

        if (groupObjects == 0) {
            if (currentGroup->size() == 1) {
                currentGroup->clear();
            } else {
                currentGroup = nullptr;
            }
        }
    }

    if (currentGroup != nullptr && currentGroup->empty()) {
        activeGroups.resize(activeGroups.size() - 1);
    }
}

void Physics::detectCollisions(CollisionGroup& collisionGroup) {
    auto firstObjectIt = collisionGroup.begin();
    auto groupEndIt = collisionGroup.end();

    for (; firstObjectIt != groupEndIt; ++firstObjectIt) {
        auto secondObjectIt = firstObjectIt;
        ++secondObjectIt;

        for (; secondObjectIt != groupEndIt; ++secondObjectIt) {
            if ((*firstObjectIt)->collidesWith(*secondObjectIt)) {
                // TODO
                (*firstObjectIt)->setCollides(true);
                (*secondObjectIt)->setCollides(true);
            }
        }
    }
}

void Physics::renderFrame(float frameTime) {
    std::lock_guard<std::mutex> lock(this->objectsMutex);

    for (RigidObject& object: this->objects) {
        object.setCollides(false);  // TODO
    }

    std::vector<CollisionGroup> activeGroups;
    this->sweepAndPrune(SweepAxis::X, activeGroups);

    for (CollisionGroup& collisionGroup: activeGroups) {
        this->detectCollisions(collisionGroup);
    }

    for (RigidObject& object: this->objects) {
        if (!object.isPinned()) {
            QVector2D acceleration(gravityForce(object) / object.getMass());

            QVector2D linearVelocityDelta(acceleration * frameTime);
            object.linearAccelerate(linearVelocityDelta);

            QVector2D positionDelta(object.getLinearVelocity() * frameTime);
            object.move(positionDelta);
        }
    }

    emit this->frameReady();
}

QVector2D Physics::gravityForce(const RigidObject& object) const {
    static QVector2D g(0.0f, -9.8f);
    return object.getMass() * g;
}
