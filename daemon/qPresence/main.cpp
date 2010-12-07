#include <QtCore/QCoreApplication>
#include "announcer.h"
#include "controlconnectionmanager.h"
#include <stdlib.h>
#include <time.h>
#include "presenceconfig.h"
#include <QSysInfo>

int main(int argc, char *argv[])
{
    PresenceConfig config;
    QCoreApplication a(argc, argv);

    // Parse command line arguments.
    if ( ! config.parseCmdLine(a.arguments()) )
    {
        // Error parsing command line arguments.
        printf("Error parsing command line arguments.\n");
        printf("Usage: qPresence [-n node_name] [-p announcement_port] [-c client_port] [-i ping_interval]\n");
        printf("node_name           The name of the presence node.\n");
        printf("announcement_port   The port number announcements are sent on.\n");
        printf("client_port         The internal port that client applications connect to.\n");
        printf("ping_interval       The number of seconds between emitting pings.\n");

        return 1;
    }

    // Create an announcer to send and receive announcements.
    Announcer announcer(config.nodeName(), config.pingInterval(), config.announcementPort());
    announcer.start();

    // Create a control connection manager.
    ControlConnectionManager ccmanager(&announcer, config.clientPort());
    ccmanager.start();

    // Print status info.
    printf("Starting presence node named %s\n", config.nodeName().data());
    printf("Announcement port: %i\n", config.announcementPort());
    printf("Client port: %i\n", config.clientPort());
    printf("Ping interval: %i\n", config.pingInterval());
    if (QSysInfo::ByteOrder == QSysInfo::BigEndian)
        printf("Running on a big-endian system.\n");
    else
        printf("Running on a little-endian system.\n");

    return a.exec();
}
