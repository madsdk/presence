#include "controlconnectionmanager.h"
#include "controlconnection.h"

ControlConnectionManager::ControlConnectionManager(Announcer *announcer, quint16 port)
{
    this->port = port;
    this->announcer = announcer;
    connect(&server, SIGNAL(newConnection()), this, SLOT(connectionEstablished()));
}

void ControlConnectionManager::start()
{
    server.listen(QHostAddress::LocalHost, port);
}

void ControlConnectionManager::connectionEstablished()
{
    printf("connection established\n");//DEBUG


    // Get the connection socket.
    QTcpSocket *socket = server.nextPendingConnection();
    if (socket == 0) return;

    // Create a new ControlConnection to handle this connection.
    new ControlConnection(this, socket, announcer);
}
