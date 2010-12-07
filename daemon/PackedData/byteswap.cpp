#include "byteswap.h"
#include <QHostInfo>

quint16 _htons(quint16 x)
{
    if (QSysInfo::ByteOrder == QSysInfo::BigEndian)
    {
        return x;
    }
    else
    {
        return BYTE_SWAP2(x);
    }
}

quint16 _ntohs(quint16 x)
{
    if (QSysInfo::ByteOrder == QSysInfo::BigEndian)
    {
        return x;
    }
    else
    {
        return BYTE_SWAP2(x);
    }
}

quint32 _htonl(quint32 x)
{
    if (QSysInfo::ByteOrder == QSysInfo::BigEndian)
    {
        return x;
    }
    else
    {
        return BYTE_SWAP4(x);
    }
}

quint32 _ntohl(quint32 x)
{
    if (QSysInfo::ByteOrder == QSysInfo::BigEndian)
    {
        return x;
    }
    else
    {
        return BYTE_SWAP4(x);
    }
}
