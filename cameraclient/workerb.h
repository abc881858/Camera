#pragma once

#include <QDebug>
#include <QImage>
#include <QObject>
#include <QThread>

class WorkerB : public QObject
{
    Q_OBJECT
public:
    explicit WorkerB(QObject *parent = nullptr);
public slots:
    void image_slot(QImage image);
signals:
    void data_b_signal(QImage image);
};
