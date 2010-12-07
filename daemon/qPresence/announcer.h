#ifndef ANNOUNCER_H
#define ANNOUNCER_H

#define MAX_PACKET_SIZE 4096

#include <QObject>
#include "announcementpacket.h"
#include <QUdpSocket>
#include <QTimer>
#include <QHostAddress>
#include "uint16.h"
#include "servicedescription.h"
#include "presencepeer.h"

class Announcer : public QObject
{
    Q_OBJECT

private:
    AnnouncementPacket packet;
    QUdpSocket sock;
    QTimer timer;
    quint16 port;

private slots:
    void sendAnnouncement();
    void receiveAnnouncement();

signals:
    void serviceAnnounced(PresencePeer sender, ServiceDescription service);

public:
    Announcer(const QByteArray &node_name, const int emit_interval, const quint16 port);
    ~Announcer() {}

    const QByteArray& nodeName() const;
    bool registerService(const ServiceDescription &service);
    bool hasService(const QByteArray &name) const;
    bool removeService(const QByteArray &name);
    bool updateService(const ServiceDescription &service);

    void start();
    void stop();
};

#endif // ANNOUNCER_H
