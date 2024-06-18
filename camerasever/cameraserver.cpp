#include "cameraserver.h"

CameraServer::CameraServer(QObject *parent)
    : QTcpServer(parent), m_socket(nullptr)
{
    parameters["brightness"] = "50";
    parameters["contrast"] = "50";
}

void CameraServer::incomingConnection(qintptr socketDescriptor)
{
    if (m_socket) {
        m_socket->disconnectFromHost();
    }

    m_socket = new QTcpSocket(this);
    m_socket->setSocketDescriptor(socketDescriptor);

    connect(m_socket, &QTcpSocket::readyRead, this, &CameraServer::readClient);
    connect(m_socket, &QTcpSocket::disconnected, this, &CameraServer::discardClient);
}

void CameraServer::readClient()
{
    m_buffer.append(m_socket->readAll());

    if (m_reading_header && m_buffer.size() >= 8) {
        QDataStream headerStream(m_buffer);
        headerStream.setByteOrder(QDataStream::BigEndian);
        headerStream >> m_header.fixedByte;
        if (m_header.fixedByte != 0x27) {
            qDebug() << "Invalid fixed byte:" << m_header.fixedByte;
            return;
        }
        headerStream >> m_header.messageType;
        headerStream >> m_header.messageId;
        headerStream >> m_header.payloadLength;

        m_reading_header = false;

        m_buffer.remove(0, 8);
    }

    if (!m_reading_header && m_buffer.size() >= int(m_header.payloadLength))
    {
        QByteArray payload = m_buffer.left(m_header.payloadLength);
        m_buffer.remove(0, m_header.payloadLength);

        QDataStream payloadStream(payload);
        payloadStream.setByteOrder(QDataStream::BigEndian);

        qDebug() << "messageType" << m_header.messageType;

        switch (m_header.messageType) {
        case SET_PARAM: {
            QString paramName;
            QString paramValue;
            payloadStream >> paramName >> paramValue;
            setParam(paramName, paramValue);
            break;
        }
        case GET_PARAM: {
            QString paramName;
            payloadStream >> paramName;
            sendParam(paramName);
            break;
        }
        case GET_IMAGE: {
            sendImage();
            break;
        }
        default:
            qDebug() << "Unknown message type:" << m_header.messageType;
        }

        m_header.fixedByte = 0;
        m_header.messageType = 0;
        m_header.messageId = 0;
        m_header.payloadLength = 0;

        m_reading_header = true;
    }
}

void CameraServer::discardClient()
{
    m_socket->deleteLater();
    m_socket = nullptr;
}

void CameraServer::setParam(const QString &paramName, const QString &paramValue)
{
    qDebug() << __func__ << paramName << paramValue;
    parameters[paramName] = paramValue;
}

void CameraServer::sendParam(const QString &paramName)
{
    if (!m_socket) return;

    QString paramValue = parameters.value(paramName, "unknown");
    qDebug() << __func__ << paramName << paramValue;

    QByteArray payload;
    QDataStream tmp(&payload, QIODevice::WriteOnly);
    tmp.setByteOrder(QDataStream::BigEndian);
    tmp << paramName << paramValue;

    QByteArray response;
    QDataStream stream(&response, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::BigEndian);
    stream << quint8(0x27) << quint8(0x02) << quint16(++m_message_id) << quint32(payload.size());

    m_socket->write(response);
    m_socket->write(payload);
    m_socket->flush();
}

void CameraServer::sendImage()
{
    qDebug() << __func__;

    if (!m_socket) return;

    QImage image(640, 480, QImage::Format_RGB32);
    image.fill(Qt::blue);

    QByteArray payload;
    QBuffer buffer(&payload);
    image.save(&buffer, "PNG");

    QByteArray response;
    QDataStream stream(&response, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::BigEndian);
    stream << quint8(0x27) << quint8(0x03) << quint16(++m_message_id) << quint32(payload.size());

    qDebug() << __func__ << "payloadLength" << payload.size();

    m_socket->write(response);
    m_socket->write(payload);
    m_socket->flush();
}
