#ifndef UINT32_H
#define UINT32_H

#include "serializable.h"
#include "byteswap.h" // For _htonl and _ntohl.
#include <string.h> // For memcpy.
#include <QtGlobal>

class UInt32 : public Serializable
{
private:
    quint32 _value;

public:
    UInt32() : _value(0) {}
    UInt32(const quint32 &value) : _value(value) {}
    UInt32(const UInt32 &other);
    UInt32& operator=(const UInt32 &other);

    quint32 value() const;
    void setValue(const quint32 &value);

    int packLength() const;
    char * pack(char *target) const;
    char * unpack(char *source, char *source_end);
};

#endif // UINT32_H
