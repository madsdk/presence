#include "maxlengthcstring.h"

MaxLengthCString::MaxLengthCString(const MaxLengthCString &other) : Serializable()
{
    _maxLength = other._maxLength;
    _str = other._str;
}

MaxLengthCString& MaxLengthCString::operator=(const MaxLengthCString& other)
{
    _maxLength = other._maxLength;
    _str = other._str;
    return *this;
}

quint16 MaxLengthCString::maxLength() const
{
    return _maxLength;
}

quint16 MaxLengthCString::length() const
{
    return _str.length();
}

bool MaxLengthCString::setStr(const QByteArray &str)
{
    // Validate length.
    if (str.length() > _maxLength) {
        return false;
    }
    _str = str;
    return true;
}

void MaxLengthCString::setStrTruncate(const QByteArray &str)
{
    if (str.length() > _maxLength)
    {
        _str = QByteArray(str.data(), _maxLength);
    }
    else
    {
        _str = str;
    }
}

const QByteArray& MaxLengthCString::str() const
{
    return _str;
}

int MaxLengthCString::packLength() const
{
    return _str.length() + sizeof(quint16);
}

char* MaxLengthCString::pack(char *target) const
{
    char *p = target;

    // Pack the size.
    UInt16 net_length(_str.length());
    p = net_length.pack(p);
    // Pack the string.
    memcpy(p, _str.data(), _str.length());
    p += _str.length();

    return p;
}

char * MaxLengthCString::unpack(char *source, char *source_end)
{
    char *p = source;
    char *q;

    // Unpack the size.
    UInt16 size;
    q = size.unpack(p, source_end);
    if (q == p || size.value() > _maxLength || source_end - source < size.value())
    {
        return source;
    }
    p = q;

    // Unpack the data.
    _str = QByteArray(p, size.value());

    return p + size.value();
}
