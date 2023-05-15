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
        buffer.write<uint8_t>(1);
        buffer.write<uint32_t>(port);
        buffer.write<std::string &>(host);
    }

    ConnectionInfo *PlainSocketConnectionInfo::parseConnectionInfo(Buffer &buffer) {
        uint32_t port = buffer.read<uint32_t>();
        std::string host = buffer.read<std::string>();
        return new PlainSocketConnectionInfo(host, port);
    }

    PlainSocketConnectionInfo::PlainSocketConnectionInfo(std::string &host, uint32_t port) : host(host), port(port) {}

    PlainSocketConnectionInfo::PlainSocketConnectionInfo(std::string &&host, uint32_t port) : host(host), port(port) {}
}