#include "announcementpacket.h"

const QList<ServiceDescription> AnnouncementPacket::services() const
{
    return _services;
}

const QByteArray& AnnouncementPacket::nodeName() const
{
    return _nodeName.name();
}

bool AnnouncementPacket::setNodeName(const QByteArray &name)
{
    return _nodeName.setName(name);
}

quint16 AnnouncementPacket::serviceCount() const
{
    return (quint16)_services.length();
}

bool AnnouncementPacket::addService(const ServiceDescription &service)
{
    // Check if the service already exists.
    if (hasService(service.name()))
    {
        return false;
    }

    // Add the service to the packet.
    _services.append(service);
    return true;
}

bool AnnouncementPacket::removeService(const QByteArray &name)
{
    for (int i = 0; i < _services.length(); i++)
    {
        if (_services[i].name() == name)
        {
            _services.removeAt(i);
            return true;
        }
    }
    return false;
}

bool AnnouncementPacket::hasService(const QByteArray &name) const
{
    foreach (ServiceDescription service, _services)
    {
        if (service.name() == name)
        {
            return true;
        }
    }
    return false;
}

bool AnnouncementPacket::updateService(const ServiceDescription &service)
{
    for (int i = 0; i < _services.length(); i++)
    {
        if (_services[i].name() == service.name())
        {
            _services.replace(i, service);
            return true;
        }
    }
    return false;
}

// ---------- Serializable interface ----------

int AnnouncementPacket::packLength() const
{
    quint16 packLength = _nodeName.packLength() + sizeof(quint16);
    QList<ServiceDescription>::const_iterator it;
    for (it = _services.begin(); it != _services.end(); it++)
    {
        packLength += (*it).packLength();
    }
    return packLength;
}

char* AnnouncementPacket::pack(char *target) const
{
    char *p = target;

    // Pack the node name.
    p = _nodeName.pack(p);

    // Pack the service count.
    UInt16 serviceCount((quint16)_services.length());
    p = serviceCount.pack(p);

    // Now pack all the service descriptions.
    foreach (ServiceDescription service, _services)
    {
        p = service.pack(p);
    }

    return p;
}

char* AnnouncementPacket::unpack(char *source, char *source_end)
{
    char *p = source;
    char *q;

    // Unpack the node name.
    q = _nodeName.unpack(p, source_end);
    if (q == p) return source;
    p = q;

    // Unpack the service count.
    UInt16 serviceCount;
    q = serviceCount.unpack(p, source_end);
    if (q == p) return source;
    p = q;

    // Unpack the services.
    for (int i = 0; i < serviceCount.value(); i++)
    {
        ServiceDescription service;
        q = service.unpack(p, source_end);
        if (q == p) return source;
        _services.append(service);
        p = q;
    }

    return p;
}
