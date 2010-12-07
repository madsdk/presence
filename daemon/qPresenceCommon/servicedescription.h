#ifndef SERVICEDESCRIPTION_H
#define SERVICEDESCRIPTION_H

#include "servicename.h"
#include "uint16.h"
#include "servicedata.h"
#include "serializable.h"

class ServiceDescription : public Serializable
{
private:
    ServiceName _name;
    UInt16 port;
    ServiceData data;

public:
    ServiceDescription() : Serializable() {}
    ServiceDescription(const ServiceName &name, quint16 port);
    ServiceDescription(const ServiceName &name, quint16 port, quint16 datalen, const char *data);
    ServiceDescription(const ServiceDescription &other);
    ServiceDescription& operator =(const ServiceDescription &other);

    const QByteArray& name() const;

    bool operator ==(const QByteArray &name) const;

    // Serializable interface.
    int packLength() const;
    char* pack(char *target) const;
    char* unpack(char *source, char *source_end);
};

#endif // SERVICEDESCRIPTION_H
