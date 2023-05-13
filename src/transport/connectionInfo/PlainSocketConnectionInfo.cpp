#include "PlainSocketConnectionInfo.h"
#include "../PlainSocketTransport.h"

namespace RingSwarm::transport {
    Transport *PlainSocketConnectionInfo::openConnection() {
        return new PlainSocketTransport(host, port);
    }

    uint32_t PlainSocketConnectionInfo::getSerializedSize() {
        return 1 + 4 + 4 + host.size();
    }

    void PlainSocketConnectionInfo::serialize(Buffer &buffer) {
        buffer.writeUint8(1);
        buffer.writeUint32(port);
        buffer.writeString(host);
    }

    ConnectionInfo *PlainSocketConnectionInfo::parseConnectionInfo(Buffer &buffer) {
        uint32_t port = buffer.readUint32();
        std::string host = buffer.readString();
        return new PlainSocketConnectionInfo(host, port);
    }

    PlainSocketConnectionInfo::PlainSocketConnectionInfo(std::string &host, uint32_t port) : host(host), port(port) {}

    PlainSocketConnectionInfo::PlainSocketConnectionInfo(std::string &&host, uint32_t port) : host(host), port(port) {}
}