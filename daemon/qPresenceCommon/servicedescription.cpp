#include "servicedescription.h"

ServiceDescription::ServiceDescription(const ServiceName &name, quint16 port) : Serializable()
{
    _name = name;
    this->port.setValue(port);
}

ServiceDescription::ServiceDescription(const ServiceName &name, quint16 port, quint16 datalen, const char *data) : Serializable()
{
    _name = name;
    this->port.setValue(port);
    this->data.setData(QByteArray(data, datalen));
}

ServiceDescription::ServiceDescription(const ServiceDescription &other) : Serializable()
{
    _name = other._name;
    port = other.port;
    data = other.data;
}

ServiceDescription& ServiceDescription::operator=(const ServiceDescription &other)
{
    _name = other._name;
    port = other.port;
    data = other.data;
    return *this;
}

bool ServiceDescription::operator ==(const QByteArray &name) const
{
    return _name.str() == name;
}

const QByteArray& ServiceDescription::name() const
{
    return _name.name();
}

// ---------- Serializable interface ----------

int ServiceDescription::packLength() const
{
    return _name.packLength() + port.packLength() + data.packLength();
}

char* ServiceDescription::pack(char *target) const
{
    char *p = target;

    // Pack the description.
    p = _name.pack(p);
    p = port.pack(p);
    p = data.pack(p);

    return p;
}

char* ServiceDescription::unpack(char *source, char *source_end)
{
    char *p = source;
    char *q;

    // Read the description.
    // Start by reading in the name.
    q = _name.unpack(p, source_end);
    if (q == p)
    {
        // The name could not be read - because the size was wrong.
        return source;
    }
    p = q;

    // Read port.
    q = port.unpack(p, source_end);
    if (q == p)
    {
        return source;
    }
    p = q;

    // Read data.
    q = data.unpack(p, source_end);
    if (q == p)
    {
        // The data could not be read - because the size was wrong.
        return source;
    }
    p = q;

    return p;
}
