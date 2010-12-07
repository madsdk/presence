#ifndef PRESENCEPEER_H
#define PRESENCEPEER_H

#include <QObject>
#include "nodename.h"
#include "uint32.h"

class PresencePeer : public QObject, public Serializable
{
    Q_OBJECT

private:
    NodeName _name;
    UInt32 _address;

public:
    PresencePeer(const QByteArray &name, const quint32 &address);
    PresencePeer(const PresencePeer &other);
    PresencePeer& operator = (const PresencePeer &other);

    const QByteArray& name() const;
    quint32 address() const;

    // Serializable interface.
    int packLength() const;
    char* pack(char *target) const;
    char* unpack(char *source, char *source_end);
};

#endif // PRESENCEPEER_H
