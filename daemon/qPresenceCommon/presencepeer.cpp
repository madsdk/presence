#include "presencepeer.h"

PresencePeer::PresencePeer(const QByteArray &name, const quint32 &address) : QObject(0), Serializable()
{
    this->_name.setName(name);
    this->_address.setValue(address);
}

PresencePeer::PresencePeer(const PresencePeer &other) : QObject(0), Serializable()
{
    _name = other._name;
    _address = other._address;
}

PresencePeer& PresencePeer::operator = (const PresencePeer &other)
{
    _name = other._name;
    _address = other._address;
    return *this;
}

const QByteArray& PresencePeer::name() const
{
    return _name.name();
}

quint32 PresencePeer::address() const
{
    return _address.value();
}

int PresencePeer::packLength() const
{
    return _address.packLength() + _name.packLength();
}

char* PresencePeer::pack(char *target) const
{
    char *p = target;

    p = _name.pack(p);
    p = _address.pack(p);

    return p;
}

char* PresencePeer::unpack(char *source, char *source_end)
{
    char *p = source;
    char *q;

    // Unpack name.
    q = _name.unpack(p, source_end);
    if (q == p) return source;
    p = q;

    // Unpack address.
    q = _address.unpack(p, source_end);
    if (q == p) return source;
    p = q;

    return p;
}
