#include "nodename.h"

NodeName::NodeName(const QByteArray &name) : MaxLengthCString(NODE_NAME_SIZE)
{
    setName(name);
}

NodeName::NodeName(const NodeName &other) : MaxLengthCString(NODE_NAME_SIZE)
{
    this->setName(other.name());
}

NodeName& NodeName::operator=(const NodeName &other)
{
    this->setName(other.name());
    return *this;
}

bool NodeName::setName(const QByteArray &name)
{
    return setStr(name);
}

const QByteArray& NodeName::name() const
{
    return str();
}
