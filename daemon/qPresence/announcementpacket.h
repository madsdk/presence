#ifndef ANNOUNCEMENTPACKET_H
#define ANNOUNCEMENTPACKET_H

#include "serializable.h"
#include <QtGlobal>
#include <QObject>
#include <QList>
#include "servicedescription.h"
#include "nodename.h"
#include "uint16.h"

class AnnouncementPacket : public QObject, public Serializable
{
    Q_OBJECT

private:
    NodeName _nodeName;
    QList<ServiceDescription> _services;

public:
    AnnouncementPacket(QObject *parent = NULL) : QObject(parent), Serializable() {}
    ~AnnouncementPacket() {}

    const QList<ServiceDescription> services() const;

    const QByteArray& nodeName() const;
    bool setNodeName(const QByteArray &name);
    quint16 serviceCount() const;
    bool addService(const ServiceDescription &service);
    bool removeService(const QByteArray &name);
    bool hasService(const QByteArray &name) const;
    bool updateService(const ServiceDescription &service);

    // Serializable interface.
    int packLength() const;
    char* pack(char *target) const;
    char* unpack(char *source, char *source_end);
};

#endif // ANNOUNCEMENTPACKET_H
