#pragma once

#include <QDebug>
#include <QImage>
#include <QObject>
#include <QThread>

class WorkerC : public QObject
{
    Q_OBJECT
public:
    explicit WorkerC(QObject *parent = nullptr);
public slots:
    void image_slot(QImage image);
signals:
    void data_c_signal(QImage image);
};
