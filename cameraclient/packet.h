#pragma once

#include <QObject>

enum MessageType
{
    SET_PARAM = 0x01,
    GET_PARAM = 0x02,
    GET_IMAGE = 0x03
};

struct PacketHeader
{
    quint8 fixedByte{0};
    quint8 messageType{0};
    quint16 messageId{0};
    quint32 payloadLength{0};
};
