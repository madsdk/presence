#include "servicedata.h"

ServiceData::ServiceData(const QByteArray& data) : MaxLengthCString(MAX_SERVICE_DATA_SIZE)
{
    setData(data);
}

ServiceData::ServiceData(const ServiceData &other) : MaxLengthCString(MAX_SERVICE_DATA_SIZE)
{
    this->setData(other.data());
}

ServiceData& ServiceData::operator=(const ServiceData &other)
{
    this->setData(other.data());
    return *this;
}

const QByteArray& ServiceData::data() const
{
    return str();
}

bool ServiceData::setData(const QByteArray& data)
{
    return setStr(data);
}
