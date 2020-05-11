#include "Canvas.h"

#include <QPaintEvent>
#include <QResizeEvent>
#include <QMouseEvent>
#include <QPainter>

Canvas::Canvas(std::vector<RigidObject>& objects, std::mutex& objectsMutex):
        QWidget(nullptr),
        objects(objects),
        objectsMutex(objectsMutex) {
}

void Canvas::resizeEvent(QResizeEvent* event) {
    /*
     * Global:
     *   X(canvas) = X(world) + canvasWidth / 2
     *   Y(canvas) = -Y(world) + canvasHeight / 2
     */
    this->globalToCanvas.reset();
    this->globalToCanvas.translate(width() / 2, height() / 2);
    this->globalToCanvas.scale(1, -1);

    /*
     * Local:
     *   X(canvas) = X(local)
     *   Y(canvas) = -Y(local)
     */
    this->localToCanvas.reset();
    this->localToCanvas.scale(1, -1);

    this->canvasToGlobal = this->globalToCanvas.inverted();
    this->canvasToLocal = this->localToCanvas.inverted();
}

void Canvas::mousePressEvent(QMouseEvent* event) {
    this->lastPosition = event->localPos().toPoint();
    std::lock_guard<std::mutex> lock(this->objectsMutex);

    for (RigidObject& object: this->objects) {
        QPoint objectPosition(this->globalToCanvas.map(object.getPosition().toPoint()));

        QPolygon vertices(object.getVertices());
        vertices.translate(objectPosition);

        if (vertices.containsPoint(this->lastPosition, Qt::FillRule::OddEvenFill)) {
            this->selectedObject = &object;
            break;
        }
    }
}

void Canvas::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::MouseButton::LeftButton) {
        this->lastPosition = QPoint();
        this->selectedObject = nullptr;
    } else if (event->button() == Qt::MouseButton::RightButton) {
        if (this->selectedObject != nullptr) {
            std::lock_guard<std::mutex> lock(this->objectsMutex);
            this->selectedObject->setPinned(!this->selectedObject->isPinned());
        }
    }
}

void Canvas::mouseMoveEvent(QMouseEvent* event) {
    QPoint localPosition(event->localPos().toPoint());
    QPoint deltaPosition(localPosition - this->lastPosition);

    if (this->selectedObject != nullptr) {
        deltaPosition = this->canvasToLocal.map(deltaPosition);
        std::lock_guard<std::mutex> lock(this->objectsMutex);

        QVector2D position(this->selectedObject->getPosition());
        position += QVector2D(deltaPosition);
        this->selectedObject->setPosition(position);
    }

    this->lastPosition = localPosition;
}

void Canvas::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setPen(Qt::black);

    // Refresh screen
    painter.setBrush(Qt::white);
    painter.drawRect(0, 0, width() - 1, height() - 1);

    // Lockless
    for (RigidObject& object: this->objects) {
        QPoint position(this->globalToCanvas.map(object.getPosition().toPoint()));
        QPolygon vertices(this->localToCanvas.map(object.getVertices()));
        QBrush brush(object.isCollides() ? Qt::red : object.getColor());

        painter.save();

        painter.setBrush(brush);
        painter.translate(position);
        painter.drawPolygon(vertices);

        painter.restore();
    }

    emit this->frameReady();
}
