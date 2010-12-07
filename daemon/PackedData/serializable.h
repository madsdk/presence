#ifndef SERIALIZABLE_H
#define SERIALIZABLE_H

class Serializable
{
public:
    // Dummy destructor to make C++ happy.
    virtual ~Serializable() {};

    // Interface definition.
    virtual int packLength() const = 0;
    virtual char* pack(char *target) const = 0;
    virtual char* unpack(char *source, char *source_end) = 0;
};

#endif // SERIALIZABLE_H
