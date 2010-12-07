#ifndef BYTESWAP_H
#define BYTESWAP_H

#include <QtGlobal>

#define BYTE_SWAP4(x) \
    (((x & 0xFF000000) >> 24) | \
     ((x & 0x00FF0000) >> 8)  | \
     ((x & 0x0000FF00) << 8)  | \
     ((x & 0x000000FF) << 24))

#define BYTE_SWAP2(x) \
    (((x & 0xFF00) >> 8) | \
     ((x & 0x00FF) << 8))

quint16 _htons(quint16 x);
quint16 _ntohs(quint16 x);
quint32 _htonl(quint32 x);
quint32 _ntohl(quint32 x);

#endif // BYTESWAP_H
