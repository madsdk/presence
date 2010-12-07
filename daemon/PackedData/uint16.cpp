#include "uint16.h"

UInt16::UInt16(const UInt16 &other) : Serializable()
{
    _value = other._value;
}

UInt16& UInt16::operator=(const UInt16 &other)
{
    _value = other._value;
    return *this;
}

quint16 UInt16::value() const
{
    return _value;
}

void UInt16::setValue(const quint16 &value)
{
    _value = value;
}

int UInt16::packLength() const
{
    return sizeof(quint16);
}

char * UInt16::pack(char *target) const
{
    quint16 net_value = _htons(_value);
    memcpy(target, &net_value, sizeof(quint16));
    return target + sizeof(quint16);
}

char * UInt16::unpack(char *source, char *source_end)
{
    // Check boundaries.
    if (source_end - source < packLength())
    {
        return source;
    }

    _value = _ntohs(*((quint16*)source));
    return source + sizeof(quint16);
}
