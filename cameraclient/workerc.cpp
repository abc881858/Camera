#include "workerc.h"

WorkerC::WorkerC(QObject *parent)
    : QObject{parent}
{
}

void WorkerC::image_slot(QImage image)
{
    qDebug() << "WorkerC processing data:" << image.size();
    QThread::sleep(1); // 模拟处理耗时
    QImage new_image(640, 480, QImage::Format_RGB32);
    new_image.fill(Qt::red);
    emit data_c_signal(new_image);
}
