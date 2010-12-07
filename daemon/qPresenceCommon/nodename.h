#ifndef NODENAME_H
#define NODENAME_H

#define NODE_NAME_SIZE 32

#include "maxlengthcstring.h"
#include <QByteArray>

class NodeName : public MaxLengthCString
{
public:
    NodeName() : MaxLengthCString(NODE_NAME_SIZE) {}
    NodeName(const QByteArray &name);
    NodeName(const NodeName &other);
    NodeName& operator=(const NodeName &other);

    bool setName(const QByteArray &name);
    const QByteArray& name() const;
};

#endif // NODENAME_H
