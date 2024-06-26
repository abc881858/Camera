#pragma once

#include <QMainWindow>
#include "cameraclient.h"
#include <QThread>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private:
    Ui::MainWindow *ui;
    CameraClient *m_camera_client;
    QThread m_camera_thread;
public slots:
    void image_slot(QImage image);
    void brightness_slot(QString str);
private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_4_clicked();
signals:
    void connectToServer(QString, quint16);
    void requestImage();
    void setParam(QString, QString);
    void getParam(QString);
};
