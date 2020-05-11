#ifndef CANVAS_H
#define CANVAS_H

#include "RigidObject.h"

#include <QWidget>
#include <QPoint>
#include <QMatrix>
#include <vector>
#include <mutex>

class Canvas: public QWidget {
    Q_OBJECT

public:
    Canvas(std::vector<RigidObject>& objects, std::mutex& objectsMutex);

protected:
    void resizeEvent(QResizeEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

signals:
    void frameReady();

private:
    std::vector<RigidObject>& objects;
    std::mutex& objectsMutex;

    QMatrix globalToCanvas;
    QMatrix localToCanvas;
    QMatrix canvasToGlobal;
    QMatrix canvasToLocal;

    QPoint lastPosition;
    RigidObject* selectedObject;
};

#endif // CANVAS_H
