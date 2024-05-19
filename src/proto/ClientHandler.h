#ifndef RINGSWARM_CLIENTHANDLER_H
#define RINGSWARM_CLIENTHANDLER_H

#include "../core/Node.h"
#include "../transport/Transport.h"

#include "ClientTransport.h"

namespace RingSwarm::proto {
class ClientHandler {
    ClientTransport clientSideWrapper;

    core::Node &remote;

public:
    ClientHandler(
        std::unique_ptr<transport::Transport> transport,
        core::Node &remote
    );

    core::Node &getRemote();
};
}// namespace RingSwarm::proto

#endif// RINGSWARM_CLIENTHANDLER_H
