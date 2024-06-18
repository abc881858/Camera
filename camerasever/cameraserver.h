#pragma once

#include <QBuffer>
#include <QDataStream>
#include <QDebug>
#include <QImage>
#include <QMap>
#include <QTcpServer>
#include <QTcpSocket>
#include "packet.h"

class CameraServer : public QTcpServer
{
    Q_OBJECT
public:
    CameraServer(QObject *parent = nullptr);
    void sendParam(const QString &paramName);
    void sendImage();
protected:
    void incomingConnection(qintptr socketDescriptor) override;
private:
    QTcpSocket *m_socket;
    QByteArray m_buffer;
    quint16 m_message_id{0};
    PacketHeader m_header;
    bool m_reading_header{true};
    QMap<QString, QString> parameters;
private slots:
    void readClient();
    void discardClient();
    void setParam(const QString &paramName, const QString &paramValue);
};
