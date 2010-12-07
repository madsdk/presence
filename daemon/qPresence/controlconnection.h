#ifndef CONTROLCONNECTION_H
#define CONTROLCONNECTION_H

#define MAX_COMMAND_SIZE 256

#include <QObject>
#include <QSet>
#include <QTcpSocket>
#include "servicedescription.h"
#include "maxlengthcstring.h"
#include "nodename.h"
#include "announcer.h"
#include "presencepeer.h"

class ControlConnection : public QObject
{
    Q_OBJECT

    enum command_t {
        STOP,
        REGISTER_CALLBACK,
        REMOVE_CALLBACK,
        REGISTER_SERVICE,
        REMOVE_SERVICE,
        CMD_ERROR,
        ACK,
        ANNOUNCEMENT,
        NODE_NAME
    };

    enum error_t {
        INVALID_MESSAGE,
        INVALID_COMMAND,
        INVALID_CONNECTION,
        SERVICE_EXISTS,
        ERR_NO_SUCH_CALLBACK,
        ERR_NO_SUCH_SERVICE,
        ERR_REGISTERING_SERVICE,
        ERR_REMOVING_SERVICE
    };

private:
    QTcpSocket *socket;
    QSet<QByteArray> callbacks;
    QSet<QByteArray> myServices;
    Announcer *announcer;

    void connectSignals();
    void disconnectSignals();
    void cmdStop();
    void cmdNodeName();
    void cmdRegisterCallback(char *start, char *end);
    void cmdRemoveCallback(char *start, char *end);
    void cmdRegisterService(char *start, char *end);
    void cmdRemoveService(char *start, char *end);
    void reportError(error_t error_number, const QByteArray &message);
    void ack();
    void lengthPrefixedWrite(char *buffer, quint16 length);

private slots:
    void disconnected();
    void error(QAbstractSocket::SocketError socketError);
    void readyRead();

public slots:
    void serviceAnnounced(PresencePeer sender, ServiceDescription service);

public:
    ControlConnection(QObject *parent, QTcpSocket *socket, Announcer *announcer);
    ~ControlConnection();
};

#endif // CONTROLCONNECTION_H
