#ifndef SERVICEDATA_H
#define SERVICEDATA_H

#include "maxlengthcstring.h"

#define MAX_SERVICE_DATA_SIZE 200

class ServiceData : public MaxLengthCString
{
public:
    ServiceData() : MaxLengthCString(MAX_SERVICE_DATA_SIZE) {}
    ServiceData(const QByteArray& data);
    ServiceData(const ServiceData &other);
    ServiceData& operator=(const ServiceData &other);

    const QByteArray& data() const;
    bool setData(const QByteArray& data);
};

#endif // SERVICEDATA_H
