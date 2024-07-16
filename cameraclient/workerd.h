#pragma once

#include <QDebug>
#include <QImage>
#include <QObject>
#include <QThread>

class WorkerD : public QObject
{
    Q_OBJECT
public:
    explicit WorkerD(QObject *parent = nullptr);
    void set_image(QImage image);
private:
    QImage m_image;
public slots:
    void data_d_slot();
signals:
    void image_signal(QImage image);
};
