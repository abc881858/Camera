#pragma once

#include <QBuffer>
#include <QDataStream>
#include <QDebug>
#include <QHostAddress>
#include <QImage>
#include <QTcpSocket>
#include "packet.h"

class CameraClient : public QObject
{
    Q_OBJECT
public:
    CameraClient(QObject *parent = nullptr);
private:
    QTcpSocket *m_socket;
    QByteArray m_buffer;
    quint16 m_message_id{0};
    PacketHeader m_header;
    bool m_reading_header{true};
public slots:
    void connectToServer(const QString &host, quint16 port);
    void requestImage();
    void setParam(const QString &paramName, const QString &paramValue);
    void getParam(const QString &paramName);
private slots:
    void connected();
    void readServer();
signals:
    void image_signal(QImage);
    void brightness_signal(QString);
};
