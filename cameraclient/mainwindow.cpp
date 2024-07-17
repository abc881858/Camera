#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    workerA.moveToThread(&threadA);
    workerB.moveToThread(&threadB);
    workerC.moveToThread(&threadC);
    workerD.moveToThread(&threadD);

    connect(this, &MainWindow::connectToServer, &workerA, &CameraClient::connectToServer);
    connect(this, &MainWindow::requestImage, &workerA, &CameraClient::requestImage);
    connect(this, &MainWindow::setParam, &workerA, &CameraClient::setParam);
    connect(this, &MainWindow::getParam, &workerA, &CameraClient::getParam);
    connect(&workerA, &CameraClient::brightness_signal, this, &MainWindow::brightness_slot);
    connect(&workerA, &CameraClient::image_signal, &workerB, &WorkerB::image_slot);
    connect(&workerA, &CameraClient::image_signal, &workerC, &WorkerC::image_slot);
    connect(&workerB, &WorkerB::data_b_signal, this, &MainWindow::data_b_slot);
    connect(&workerC, &WorkerC::data_c_signal, this, &MainWindow::data_c_slot);
    connect(this, &MainWindow::data_d_signal, &workerD, &WorkerD::data_d_slot);
    connect(&workerD, &WorkerD::image_signal, this, &MainWindow::image_slot);

    threadA.start();
    threadB.start();
    threadC.start();
    threadD.start();
}

void MainWindow::data_b_slot(QImage image)
{
    QMutexLocker locker(&mutex);
    workerBHandled = true;
    if (workerBHandled && workerCHandled)
    {
        workerD.set_image(image);
        emit data_d_signal();
    }
}

void MainWindow::data_c_slot(QImage image)
{
    QMutexLocker locker(&mutex);
    workerCHandled = true;
    if (workerBHandled && workerCHandled)
    {
        workerD.set_image(image);
        emit data_d_signal();
    }
}

MainWindow::~MainWindow()
{
    threadA.quit();
    threadA.wait();
    threadB.quit();
    threadB.wait();
    threadC.quit();
    threadC.wait();
    threadD.quit();
    threadD.wait();
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    emit connectToServer("127.0.0.1", 27000);
}

void MainWindow::on_pushButton_2_clicked()
{
    emit requestImage();
}

void MainWindow::on_pushButton_3_clicked()
{
    emit setParam("brightness", QString::number(ui->spinBox->value()));
}

void MainWindow::on_pushButton_4_clicked()
{
    emit getParam("brightness");
}

void MainWindow::image_slot(QImage image)
{
    workerBHandled = false;
    workerCHandled = false;
    ui->label->setPixmap(QPixmap::fromImage(image));
}

void MainWindow::brightness_slot(QString str)
{
    ui->spinBox->setValue(str.toInt());
}
