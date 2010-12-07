#ifndef UINT16_H
#define UINT16_H

#include "serializable.h"
#include <QtGlobal>
#include "byteswap.h"
#include <string.h> // For memcpy.

class UInt16 : public Serializable
{
private:
    quint16 _value;

public:
    UInt16() : _value(0) {}
    UInt16(const quint16 &value) : _value(value) {}
    UInt16(const UInt16 &other);
    UInt16& operator=(const UInt16 &other);

    quint16 value() const;
    void setValue(const quint16 &value);

    int packLength() const;
    char * pack(char *target) const;
    char * unpack(char *source, char *source_end);
};

#endif // UINT16_H
