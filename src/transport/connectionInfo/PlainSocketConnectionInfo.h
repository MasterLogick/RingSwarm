#ifndef RINGSWARM_PLAINSOCKETCONNECTIONINFO_H
#define RINGSWARM_PLAINSOCKETCONNECTIONINFO_H

#include "ConnectionInfo.h"
#include <string>

namespace RingSwarm::transport {
class PlainSocketConnectionInfo : public ConnectionInfo {
    std::string host;

    uint32_t port;

public:
    PlainSocketConnectionInfo(std::string &&host, uint32_t port);

    PlainSocketConnectionInfo(std::string &host, uint32_t port);

    Transport *openConnection() override;

    uint32_t getSerializedSize() override;

    void serialize(Buffer &buffer) override;

    static ConnectionInfo *parseConnectionInfo(Buffer &buffer);
};

}// namespace RingSwarm::transport

#endif//RINGSWARM_PLAINSOCKETCONNECTIONINFO_H
