#include "controlconnection.h"

ControlConnection::ControlConnection(QObject *parent, QTcpSocket *socket, Announcer *announcer) : QObject(parent)
{
    this->socket = socket;
    this->announcer = announcer;
    connectSignals();
}

ControlConnection::~ControlConnection()
{
    // Disconnect any signals.
    disconnectSignals();

    // Removed announced services.
    foreach (QByteArray service, myServices)
    {
        announcer->removeService(service);
    }

    // Clean up after the socket.
    socket->close();
    socket->deleteLater();
}

void ControlConnection::connectSignals()
{
    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(error(QAbstractSocket::SocketError)));
    connect(announcer, SIGNAL(serviceAnnounced(PresencePeer,ServiceDescription)),
            this, SLOT(serviceAnnounced(PresencePeer,ServiceDescription)));
}

void ControlConnection::disconnectSignals()
{
    disconnect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
    disconnect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    disconnect(socket, SIGNAL(error(QAbstractSocket::SocketError)),
               this, SLOT(error(QAbstractSocket::SocketError)));
    disconnect(announcer, SIGNAL(serviceAnnounced(PresencePeer,ServiceDescription)),
               this, SLOT(serviceAnnounced(PresencePeer,ServiceDescription)));
}

void ControlConnection::disconnected()
{
    deleteLater();
}

void ControlConnection::error(QAbstractSocket::SocketError socketError)
{
    if (socketError != 1)
    {
        printf("Socket error %i\n", (int)socketError);
    }
    deleteLater();
}

void ControlConnection::readyRead()
{
    char buffer[MAX_COMMAND_SIZE];
    char *data_end, *p;
    int bytes_read;

    // Read message.
    bytes_read = socket->read(buffer, MAX_COMMAND_SIZE);
    if (bytes_read <= 0) return;
    data_end = buffer + bytes_read;

    // Parse the command identifier.
    command_t command = (command_t)*((quint8*)buffer);
    p = buffer + 1;
    switch (command)
    {
    case STOP:
        cmdStop();
        break;
    case REGISTER_CALLBACK:
        cmdRegisterCallback(p, data_end);
        break;
    case REMOVE_CALLBACK:
        cmdRemoveCallback(p, data_end);
        break;
    case REGISTER_SERVICE:
        cmdRegisterService(p, data_end);
        break;
    case REMOVE_SERVICE:
        cmdRemoveService(p, data_end);
        break;
    case NODE_NAME:
        cmdNodeName();
        break;
    default:
        // An unknown command was sent.
        reportError(INVALID_COMMAND, "The incoming command was of invalid type.");
    }
}

void ControlConnection::cmdStop()
{
    // TODO: Implement system shutdown here.
    ack();
    deleteLater();
}

void ControlConnection::cmdNodeName()
{
    char buffer[MAX_COMMAND_SIZE];
    MaxLengthCString response(NODE_NAME_SIZE);

    buffer[0] = (quint8)NODE_NAME;

    response.setStr(announcer->nodeName());
    response.pack(&buffer[1]);
    lengthPrefixedWrite(buffer, response.packLength() + 1);
}

void ControlConnection::cmdRegisterCallback(char *start, char *end)
{
    // Read the service name to register for.
    MaxLengthCString service_name(SERVICE_NAME_SIZE);
    char *p = service_name.unpack(start, end);
    if (p == start)
    {
        reportError(INVALID_MESSAGE, "Error parsing service name from message.");
        return;
    }

    // Register the callback.
    callbacks.insert(service_name.str());

    // Acknowledge the successful completion of the command.
    ack();
}

void ControlConnection::cmdRemoveCallback(char *start, char *end)
{
    // Read the service name to remove registration for.
    MaxLengthCString service_name(SERVICE_NAME_SIZE);
    char *p = service_name.unpack(start, end);
    if (p == start)
    {
        reportError(INVALID_MESSAGE, "Error parsing service name from message.");
        return;
    }

    // Remove the callback.
    if (callbacks.remove(service_name.str()))
    {
        // Acknowledge the successful completion of the command.
        ack();
    }
    else
    {
        // No such callback. Report the error.
        reportError(ERR_NO_SUCH_CALLBACK, "No such callback registered.");
    }
}

void ControlConnection::cmdRegisterService(char *start, char *end)
{
    // Read the service description.
    ServiceDescription service;
    char *p = service.unpack(start, end);
    if (p == start)
    {
        reportError(INVALID_MESSAGE, "Error parsing service from message.");
        return;
    }

    // Check whether the service is already registered.
    if (announcer->hasService(service.name()))
    {
        // The service already exists. Update it if it is ours.
        if (myServices.contains(service.name()))
        {
            if (announcer->updateService(service))
                ack();
            else
                reportError(ERR_REGISTERING_SERVICE, "Error updating service.");
        }
        else
        {
            reportError(SERVICE_EXISTS, "The given service is already registered by another client.");
        }
    }
    else
    {
        // A new service must be registered.
        if (announcer->registerService(service))
        {
            myServices.insert(service.name());
            ack();
        }
        else
        {
            reportError(ERR_REGISTERING_SERVICE, "Error registering service.");
        }
    }
}

void ControlConnection::cmdRemoveService(char *start, char *end)
{
    // Read the service name to remove registration for.
    MaxLengthCString service_name(SERVICE_NAME_SIZE);
    char *p = service_name.unpack(start, end);
    if (p == start)
    {
        reportError(INVALID_MESSAGE, "Error parsing service name from message.");
        return;
    }

    // Check that the service is registered by you.
    if (myServices.contains(service_name.str())) {
        if (announcer->removeService(service_name.str()))
        {
            myServices.remove(service_name.str());
            ack();
        }
        else
            reportError(ERR_REMOVING_SERVICE, "Error removing service registration.");
    }
    else // ! myServices.contains(service.name())
    {
        reportError(ERR_NO_SUCH_SERVICE, "The given service is unknown.");
    }
}

void ControlConnection::reportError(error_t error_number, const QByteArray &message)
{
    char *p, buffer[MAX_COMMAND_SIZE];
    MaxLengthCString err_msg(MAX_COMMAND_SIZE - 4);

    buffer[0] = (quint8)CMD_ERROR;
    buffer[1] = (quint8)error_number;
    err_msg.setStrTruncate(message);
    p = err_msg.pack(buffer + 2);

    lengthPrefixedWrite(buffer, p - buffer);
}

void ControlConnection::ack()
{
    char buffer = (quint8)ACK;
    lengthPrefixedWrite(&buffer, sizeof(quint8));
}

void ControlConnection::serviceAnnounced(PresencePeer sender, ServiceDescription service)
{
    if (callbacks.contains("*") || callbacks.contains(service.name()))
    {
        char buffer[MAX_COMMAND_SIZE];
        char *p = buffer + 1;

        printf("Got announcement I subscribed to from %s.\n", sender.name().data()); //DEBUG

        // Pack the message.
        buffer[0] = (quint8)ANNOUNCEMENT;
        p = sender.pack(p);
        p = service.pack(p);

        // Send the message.
        lengthPrefixedWrite(buffer, p-buffer);
    }
}

void ControlConnection::lengthPrefixedWrite(char *buffer, quint16 length)
{
    char data[MAX_COMMAND_SIZE + 2];
    char *p;

    UInt16 len(length);
    p = len.pack(data);
    memcpy(p, buffer, length);
    socket->write(data, length + len.packLength());
}
