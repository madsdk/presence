#ifndef MAXLENGTHCSTRING_H
#define MAXLENGTHCSTRING_H

#include <QByteArray>
#include "serializable.h"
#include "uint16.h"
#include <string.h> // For memcpy

class MaxLengthCString : public Serializable
{
private:
    quint16 _maxLength;
    QByteArray _str;

public:
    MaxLengthCString(const quint16 &length) : Serializable(), _maxLength(length) {}
    MaxLengthCString(const MaxLengthCString &other);
    MaxLengthCString& operator=(const MaxLengthCString& other);

    quint16 maxLength() const;
    quint16 length() const;
    bool setStr(const QByteArray &str);
    void setStrTruncate(const QByteArray &str);
    const QByteArray& str() const;

    int packLength() const;
    char* pack(char *target) const;
    char* unpack(char *source, char *source_end);
};

#endif // MAXLENGTHCSTRING_H
