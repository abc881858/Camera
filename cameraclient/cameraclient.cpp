#include "cameraclient.h"

CameraClient::CameraClient(QObject *parent)
    : QObject(parent)
{
    m_socket = new QTcpSocket(this);
    connect(m_socket, &QTcpSocket::connected, this, &CameraClient::connected);
    connect(m_socket, &QTcpSocket::readyRead, this, &CameraClient::readServer);
}

void CameraClient::connectToServer(const QString &host, quint16 port)
{
    m_socket->connectToHost(QHostAddress(host), port);
}

void CameraClient::requestImage()
{
    if (m_socket->state() == QAbstractSocket::ConnectedState)
    {
        QByteArray request;
        QDataStream stream(&request, QIODevice::WriteOnly);
        stream.setByteOrder(QDataStream::BigEndian);
        stream << quint8(0x27) << quint8(0x03) << quint16(++m_message_id) << quint32(0);

        qDebug() << "request" << request.toHex(' ');
        qDebug() << "request size" << request.size();

        m_socket->write(request);
        m_socket->flush();
    }
}

void CameraClient::setParam(const QString &paramName, const QString &paramValue)
{
    if (m_socket->state() == QAbstractSocket::ConnectedState)
    {
        QByteArray payload;
        QDataStream stream(&payload, QIODevice::WriteOnly);
        stream.setByteOrder(QDataStream::BigEndian);
        stream << paramName << paramValue;

        QByteArray request;
        QDataStream requestStream(&request, QIODevice::WriteOnly);
        requestStream.setByteOrder(QDataStream::BigEndian);
        requestStream << quint8(0x27) << quint8(0x01) << quint16(++m_message_id) << payload.size();

        qDebug() << "payload" << payload.toHex(' ');
        qDebug() << "payload size" << payload.size();

        m_socket->write(request);
        m_socket->write(payload);
        m_socket->flush();
    }
}

void CameraClient::getParam(const QString &paramName)
{
    if (m_socket->state() == QAbstractSocket::ConnectedState)
    {
        QByteArray payload;
        QDataStream stream(&payload, QIODevice::WriteOnly);
        stream.setByteOrder(QDataStream::BigEndian);
        stream << paramName;

        QByteArray request;
        QDataStream requestStream(&request, QIODevice::WriteOnly);
        requestStream.setByteOrder(QDataStream::BigEndian);
        requestStream << quint8(0x27) << quint8(0x02) << quint16(++m_message_id) << payload.size();

        qDebug() << "payload" << payload.toHex(' ');
        qDebug() << "payload size" << payload.size();

        m_socket->write(request);
        m_socket->write(payload);
        m_socket->flush();
    }
}

void CameraClient::connected()
{
    qDebug() << __func__;
}

void CameraClient::readServer()
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

        switch (m_header.messageType) {
        case SET_PARAM: {
            qDebug() << "Set parameter response received, messageId:" << m_header.messageId;
            break;
        }
        case GET_PARAM: {
            QDataStream payloadStream(payload);
            payloadStream.setByteOrder(QDataStream::BigEndian);
            QString paramName;
            QString paramValue;
            payloadStream >> paramName >> paramValue;
            qDebug() << "Get parameter response received, messageId:" << m_header.messageId << "paramName:" << paramName << "paramValue:" << paramValue;
            if(paramName == "brightness")
            {
                emit brightness_signal(paramValue);
            }
            break;
        }
        case GET_IMAGE: {
            QImage image(640, 480, QImage::Format_RGB32);
            image.loadFromData(payload, "PNG");
            qDebug() << "Image received, messageId:" << m_header.messageId << "Image size:" << image.size();
            emit send_image(image);
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
