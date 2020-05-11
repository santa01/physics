#ifndef PHYSICS_H
#define PHYSICS_H

#include "RigidObject.h"

#include <QObject>
#include <vector>
#include <mutex>
#include <unordered_set>

enum class BoxBound { Begin, End };
enum class SweepAxis { X, Y };

class BoundingPoint {
public:
    BoundingPoint(BoxBound bound, RigidObject* object);

    int getCoordinate(SweepAxis sweepAxis) const;
    BoxBound getBound() const;
    RigidObject* getObject() const;

private:
    BoxBound bound;
    RigidObject* object;
};

class Physics: public QObject {
    Q_OBJECT

public:
    Physics(std::vector<RigidObject>& objects, std::mutex& objectsMutex);

public slots:
    void renderFrame(float frameTime);

signals:
    void frameReady();

private:
    typedef std::unordered_set<RigidObject*> CollisionGroup;
    void sweepAndPrune(SweepAxis sweepAxis, std::vector<CollisionGroup>& activeGroups);
    void detectCollisions(CollisionGroup& collisionGroup);

    void sweepBoundingPoints();
    void sortBoundingPoints(SweepAxis sweepAxis);
    void pruneBoundingPoints(std::vector<CollisionGroup>& activeGroups);

    QVector2D gravityForce(const RigidObject& object) const;

    std::vector<BoundingPoint> boundingPoints;  // Keep sorted
    std::vector<RigidObject>& objects;
    std::mutex& objectsMutex;
};

#endif // PHYSICS_H
