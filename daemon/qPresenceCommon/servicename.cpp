#include "servicename.h"

ServiceName::ServiceName(const QByteArray &name) : MaxLengthCString(SERVICE_NAME_SIZE)
{
    setName(name);
}

ServiceName::ServiceName(const ServiceName &other) : MaxLengthCString(SERVICE_NAME_SIZE)
{
    this->setName(other.name());
}

ServiceName& ServiceName::operator=(const ServiceName &other)
{
    this->setName(other.name());
    return *this;
}

bool ServiceName::setName(const QByteArray &name)
{
    return setStr(name);
}

const QByteArray& ServiceName::name() const
{
    return str();
}
