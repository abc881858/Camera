#include <QCoreApplication>
#include "cameraserver.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    CameraServer server;
    server.listen(QHostAddress::AnyIPv4, 27000);

    return a.exec();
}
