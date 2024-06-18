#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_camera_client = new CameraClient;
    connect(m_camera_client, &CameraClient::send_image, this, &MainWindow::send_image);
    connect(m_camera_client, &CameraClient::brightness_signal, this, &MainWindow::brightness_slot);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    m_camera_client->connectToServer("127.0.0.1", 27000);
}

void MainWindow::on_pushButton_2_clicked()
{
    m_camera_client->requestImage();
}

void MainWindow::on_pushButton_3_clicked()
{
    m_camera_client->setParam("brightness", QString::number(ui->spinBox->value()));
}

void MainWindow::on_pushButton_4_clicked()
{
    m_camera_client->getParam("brightness");
}

void MainWindow::send_image(QImage image)
{
    ui->label->setPixmap(QPixmap::fromImage(image));
}

void MainWindow::brightness_slot(QString str)
{
    ui->spinBox->setValue(str.toInt());
}
