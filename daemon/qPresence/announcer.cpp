#include "announcer.h"

Announcer::Announcer(const QByteArray &node_name, const int emit_interval, const quint16 port) : QObject(0)
{
    this->port = port;
    sock.bind(port);
    packet.setNodeName(node_name);
    timer.setInterval(emit_interval * 1000);
    timer.setSingleShot(false);
    connect(&timer, SIGNAL(timeout()), this, SLOT(sendAnnouncement()));
    connect(&sock, SIGNAL(readyRead()), this, SLOT(receiveAnnouncement()));
}

const QByteArray& Announcer::nodeName() const
{
    return packet.nodeName();
}

void Announcer::start()
{
    timer.start();
}

void Announcer::stop()
{
    timer.stop();
}

void Announcer::sendAnnouncement()
{
    char buffer[MAX_PACKET_SIZE];
    char *p;

    // Check whether an announcement should be sent.
    if (packet.serviceCount() == 0) return;

    // Get the pack length. This must be written in the beginning of the packet.
    UInt16 packetLength(packet.packLength());
    if (packetLength.value() > MAX_PACKET_SIZE - packetLength.packLength())
    {
        // TODO: The announcement packet is too large.
        return;
    }
    p = packetLength.pack(buffer);

    // Pack the packet into the buffer.
    p = packet.pack(p);

    // Send the packet.
    sock.writeDatagram(buffer, p - buffer, QHostAddress::Broadcast, port);
}

void Announcer::receiveAnnouncement()
{
    char buffer[MAX_PACKET_SIZE];
    char *data_end = buffer;
    char *p, *q;
    int bytes_read;
    UInt16 packet_size;

    // The readyRead signal has been emitted so data can be read.
    // Read the packet size.
    QHostAddress sender_address;
    bytes_read = sock.readDatagram(buffer, MAX_PACKET_SIZE, &sender_address);
    data_end += bytes_read;
    if (bytes_read < packet_size.packLength()) return;
    p = packet_size.unpack(buffer, data_end);
    if (p == buffer || packet_size.value() > MAX_PACKET_SIZE) return;

    // Wait for packet_size bytes to become available.
    while (data_end - p < packet_size.value())
    {
        if (sock.waitForReadyRead(100))
        {
            // Data has arrived.
            bytes_read = sock.readDatagram(data_end, packet_size.value() - (data_end - p));
            if (bytes_read <= 0) return;
            data_end += bytes_read;
        }
        else
        {
            // Nothing arrived.
            return;
        }
    }

    // The entire packet has been received.
    AnnouncementPacket new_packet;
    q = new_packet.unpack(p, data_end);
    if (q == p) return;

    // Emit the service announcements.
    foreach (ServiceDescription service, new_packet.services())
    {
        emit serviceAnnounced(PresencePeer(new_packet.nodeName(), sender_address.toIPv4Address()), service);
    }
}

bool Announcer::registerService(const ServiceDescription &service)
{
    return packet.addService(service);
}

bool Announcer::hasService(const QByteArray &name) const
{
    return packet.hasService(name);
}

bool Announcer::removeService(const QByteArray &name)
{
    return packet.removeService(name);
}

bool Announcer::updateService(const ServiceDescription &service)
{
    return packet.updateService(service);
}
