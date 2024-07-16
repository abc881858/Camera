#include "workerd.h"

WorkerD::WorkerD(QObject *parent)
    : QObject{parent}
{
}

void WorkerD::set_image(QImage image)
{
    m_image = image;
}

void WorkerD::data_d_slot()
{
    qDebug() << "WorkerD started";
    // 模拟执行任务
    QThread::sleep(1); // 模拟任务执行耗时
    emit image_signal(m_image);
}
