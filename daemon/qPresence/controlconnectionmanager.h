#ifndef CONTROLCONNECTIONMANAGER_H
#define CONTROLCONNECTIONMANAGER_H

#include <QObject>
#include <QTcpServer>
#include "announcer.h"

class ControlConnectionManager : public QObject
{
    Q_OBJECT

private:
    QTcpServer server;
    Announcer *announcer;
    quint16 port;

public slots:
    void connectionEstablished();

public:
    ControlConnectionManager(Announcer *announcer, quint16 port);

    void start();
};

#endif // CONTROLCONNECTIONMANAGER_H
