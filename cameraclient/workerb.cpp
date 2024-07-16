#include "workerb.h"

WorkerB::WorkerB(QObject *parent)
    : QObject{parent}
{
}

void WorkerB::image_slot(QImage image)
{
    qDebug() << "WorkerB processing data:" << image.size();
    QThread::sleep(1); // 模拟处理耗时
    QImage new_image(640, 480, QImage::Format_RGB32);
    new_image.fill(Qt::green);
    emit data_b_signal(new_image);
}
