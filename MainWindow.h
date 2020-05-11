#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "RigidObject.h"
#include "Physics.h"
#include "Canvas.h"

#include <QMainWindow>
#include <QVBoxLayout>
#include <QThread>
#include <vector>
#include <mutex>
#include <chrono>

namespace Ui {
class MainWindow;
}

class MainWindow: public QMainWindow {
    Q_OBJECT

public:
    MainWindow();
    ~MainWindow();

    void beginFrame();

public slots:
    void endFrame();

signals:
    void renderFrame(float frameTime);

private:
    void generateObjects(int size, int count);

    Ui::MainWindow* ui;
    QVBoxLayout layout;
    Canvas canvas;

    std::vector<RigidObject> objects;
    std::mutex objectsMutex;

    QThread physicsThread;
    Physics physics;

    std::chrono::time_point<std::chrono::steady_clock> timestamp;
    float frameTime = 0.0f;
    float timeScale = 3.0f;
};

#endif // MAINWINDOW_H
