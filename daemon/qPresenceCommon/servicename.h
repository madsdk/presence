#ifndef SERVICENAME_H
#define SERVICENAME_H

#define SERVICE_NAME_SIZE 32

#include <QByteArray>
#include "maxlengthcstring.h"

class ServiceName : public MaxLengthCString
{
public:
    ServiceName() : MaxLengthCString(SERVICE_NAME_SIZE) {}
    ServiceName(const QByteArray &name);
    ServiceName(const ServiceName &other);
    ServiceName& operator=(const ServiceName &other);

    bool setName(const QByteArray &name);
    const QByteArray& name() const;
};

#endif // SERVICENAME_H
