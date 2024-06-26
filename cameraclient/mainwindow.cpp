#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_camera_client = new CameraClient;
    m_camera_client->moveToThread(&m_camera_thread);
    connect(&m_camera_thread, &QThread::finished, m_camera_client, &CameraClient::deleteLater);
    connect(this, &MainWindow::connectToServer, m_camera_client, &CameraClient::connectToServer);
    connect(this, &MainWindow::requestImage, m_camera_client, &CameraClient::requestImage);
    connect(this, &MainWindow::setParam, m_camera_client, &CameraClient::setParam);
    connect(this, &MainWindow::getParam, m_camera_client, &CameraClient::getParam);
    connect(m_camera_client, &CameraClient::image_signal, this, &MainWindow::image_slot);
    connect(m_camera_client, &CameraClient::brightness_signal, this, &MainWindow::brightness_slot);
    m_camera_thread.start();
}

MainWindow::~MainWindow()
{
    m_camera_thread.quit();
    m_camera_thread.wait();
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
    ui->label->setPixmap(QPixmap::fromImage(image));
}

void MainWindow::brightness_slot(QString str)
{
    ui->spinBox->setValue(str.toInt());
}
