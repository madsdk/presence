#include "presenceconfig.h"
#include <time.h> // time
#include <stdlib.h> // srand, rand
#include <stdio.h> // sprintf
#include "nodename.h" // NODE_NAME_SIZE

PresenceConfig::PresenceConfig()
{
    char node_name[NODE_NAME_SIZE];

    // Create a random node name.
    srand(time(0));
    memset(node_name, 0, NODE_NAME_SIZE);
    sprintf(node_name, "presencenode-%i", rand());
    _nodeName = QByteArray(node_name, strlen(node_name));

    // Set ports.
    _announcementPort = ANNOUNCEMENT_PORT;
    _clientPort = CONTROL_PORT;

    // Set ping interval.
    _pingInterval = DEFAULT_PING_INTERVAL;
}

const QByteArray& PresenceConfig::nodeName() const
{
    return _nodeName;
}

bool PresenceConfig::setNodeName(const QByteArray &nodeName)
{
    if (nodeName.length() > NODE_NAME_SIZE)
    {
        return false;
    }
    _nodeName = nodeName;
    return true;
}

int PresenceConfig::pingInterval() const
{
    return _pingInterval;
}

bool PresenceConfig::setPingInterval(int pingInterval)
{
    if (pingInterval < 1 || pingInterval > 3600)
    {
        return false;
    }
    _pingInterval = pingInterval;
    return true;
}

quint16 PresenceConfig::announcementPort() const
{
    return _announcementPort;
}

bool PresenceConfig::setAnnouncementPort(quint16 announcementPort)
{
    if (announcementPort < 1024)
    {
        return false;
    }
    _announcementPort = announcementPort;
    return true;
}

quint16 PresenceConfig::clientPort() const
{
    return _clientPort;
}

bool PresenceConfig::setClientPort(quint16 clientPort)
{
    if (clientPort < 1024)
    {
        return false;
    }
    _clientPort = clientPort;
    return true;
}

bool PresenceConfig::parseCmdLine(const QStringList &args)
{
    for (int i = 1; i < args.length(); i += 2)
    {
        if (args[i] == "-n")
        {
            if ( ! setNodeName(args[i+1].toAscii()) )
            {
                printf("Invalid node name given.\n");
                return false;
            }
        }
        else if (args[i] == "-p")
        {
            if ( ! setAnnouncementPort(args[i+1].toInt()))
            {
                printf("Invalid announcement port given.\n");
                return false;
            }
        }
        else if (args[i] == "-c")
        {
            if ( ! setClientPort(args[i+1].toInt()))
            {
                printf("Invalid client port given.\n");
                return false;
            }
        }
        else if (args[i] == "-i")
        {
            if ( ! setPingInterval(args[i+1].toInt()))
            {
                printf("Invalid ping interval given.\n");
                return false;
            }
        }
        else
        {
            return false;
        }
    }

    return true;
}
