#include "MainWindow.h"
#include "ui_MainWindow.h"

#include <QTime>
#include <QVector2D>
#include <random>
#include <cassert>

MainWindow::MainWindow():
        QMainWindow(nullptr),
        ui(new Ui::MainWindow),
        canvas(this->objects, this->objectsMutex),
        physics(this->objects, this->objectsMutex) {
    this->ui->setupUi(this);

    this->layout.setMargin(1);
    this->layout.addWidget(&this->canvas);
    this->centralWidget()->setLayout(&this->layout);

    this->physics.moveToThread(&this->physicsThread);
    this->physicsThread.start();

    this->connect(this, &MainWindow::renderFrame, &this->physics, &Physics::renderFrame);
    this->connect(&this->physics, &Physics::frameReady, &this->canvas, QOverload<>::of(&Canvas::update));
    this->connect(&this->canvas, &Canvas::frameReady, this, &MainWindow::endFrame);

    this->generateObjects(50, 10);
    this->beginFrame();
}

MainWindow::~MainWindow() {
    this->physicsThread.quit();
    this->physicsThread.wait();

    delete this->ui;
}

void MainWindow::beginFrame() {
    this->timestamp = std::chrono::steady_clock::now();
    emit this->renderFrame(this->frameTime * this->timeScale);
}

void MainWindow::endFrame() {
    std::chrono::duration<float> duration(std::chrono::steady_clock::now() - this->timestamp);
    this->frameTime = duration.count();
    this->beginFrame();
}

void MainWindow::generateObjects(int size, int count) {
    int positionX = -width() / 2;
    int objectSpacing = (width() / count - size) * count / (count + 1);
    assert(objectSpacing > 0);

    static std::mt19937 generator(QTime::currentTime().msec());
    std::uniform_int_distribution<int> randomPosition(size, height() / 2 - size);

    for (int i = 0; i < count; i++) {
        int objectPositionX = positionX + size / 2 + objectSpacing;
        QVector2D position(objectPositionX, randomPosition(generator));

        RigidObject object(size, size);
        object.setPosition(position);
        object.setPinned(true);

        this->objects.push_back(object);
        positionX = objectPositionX + size / 2;
    }
}
