#ifndef PRESENCECONFIG_H
#define PRESENCECONFIG_H

#define DEFAULT_PING_INTERVAL 1
#define ANNOUNCEMENT_PORT 12345
#define CONTROL_PORT 2000

#include <QString>
#include <QStringList>

class PresenceConfig
{
private:
    QByteArray _nodeName;
    quint16 _announcementPort; // port used for announcements.
    quint16 _clientPort; // port used for client comm.
    int _pingInterval; // The interval between pings in seconds.

public:
    PresenceConfig();

    const QByteArray& nodeName() const;
    bool setNodeName(const QByteArray &nodeName);
    int pingInterval() const;
    bool setPingInterval(int pingInterval);
    quint16 announcementPort() const;
    bool setAnnouncementPort(quint16 announcementPort);
    quint16 clientPort() const;
    bool setClientPort(quint16 clientPort);

    bool parseCmdLine(const QStringList &args);
};

#endif // PRESENCECONFIG_H
