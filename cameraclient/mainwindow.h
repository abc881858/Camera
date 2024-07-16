#pragma once

#include <QMainWindow>
#include <QMutex>
#include <QMutexLocker>
#include <QThread>
#include "cameraclient.h"
#include "workerb.h"
#include "workerc.h"
#include "workerd.h"

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
    CameraClient workerA;
    WorkerB workerB;
    WorkerC workerC;
    WorkerD workerD;
    QThread threadA;
    QThread threadB;
    QThread threadC;
    QThread threadD;
    QMutex mutex;
    bool workerBHandled{false};
    bool workerCHandled{false};
public slots:
    void image_slot(QImage image);
    void brightness_slot(QString str);
    void data_b_slot(QImage image);
    void data_c_slot(QImage image);
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
    void data_d_signal();
};
