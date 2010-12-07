#include "uint32.h"

UInt32::UInt32(const UInt32 &other) : Serializable()
{
    _value = other._value;
}

UInt32& UInt32::operator=(const UInt32 &other)
{
    _value = other._value;
    return *this;
}

quint32 UInt32::value() const
{
    return _value;
}

void UInt32::setValue(const quint32 &value)
{
    _value = value;
}

int UInt32::packLength() const
{
    return sizeof(quint32);
}

char * UInt32::pack(char *target) const
{
    quint32 net_value = _htonl(_value);
    memcpy(target, &net_value, sizeof(quint32));
    return target + sizeof(quint32);
}

char * UInt32::unpack(char *source, char *source_end)
{
    // Check boundaries.
    if (source_end - source < packLength())
    {
        return source;
    }

    _value = _ntohl(*((quint32*)source));
    return source + sizeof(quint32);
}
