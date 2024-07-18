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

    connect(this, &MainWindow::connectToServer, &workerA, &CameraClient::connectToServer);
    connect(this, &MainWindow::requestImage, &workerA, &CameraClient::requestImage);
    connect(this, &MainWindow::setParam, &workerA, &CameraClient::setParam);
    connect(this, &MainWindow::getParam, &workerA, &CameraClient::getParam);
    connect(&workerA, &CameraClient::brightness_signal, this, &MainWindow::brightness_slot);
    connect(&workerA, &CameraClient::image_signal, &workerB, &WorkerB::image_slot);
    connect(&workerA, &CameraClient::image_signal, &workerC, &WorkerC::image_slot);
    connect(&workerB, &WorkerB::data_b_signal, this, &MainWindow::data_b_slot);
    connect(&workerC, &WorkerC::data_c_signal, this, &MainWindow::data_c_slot);

    threadA.start();
    threadB.start();
    threadC.start();
}

void MainWindow::data_b_slot(QImage image)
{
    QMutexLocker locker(&mutex);
    workerBHandled = true;
    m_image1 = image;
    if (workerBHandled && workerCHandled)
    {
        workerBHandled = false;
        workerCHandled = false;
        ui->label->setPixmap(QPixmap::fromImage(m_image1));
        ui->label_2->setPixmap(QPixmap::fromImage(m_image2));
        frameCount++;
        qDebug() << "frame" << frameCount;
    }
}

void MainWindow::data_c_slot(QImage image)
{
    QMutexLocker locker(&mutex);
    workerCHandled = true;
    m_image2 = image;
    if (workerBHandled && workerCHandled)
    {
        workerBHandled = false;
        workerCHandled = false;
        ui->label->setPixmap(QPixmap::fromImage(m_image1));
        ui->label_2->setPixmap(QPixmap::fromImage(m_image2));
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

void MainWindow::brightness_slot(QString str)
{
    ui->spinBox->setValue(str.toInt());
}
