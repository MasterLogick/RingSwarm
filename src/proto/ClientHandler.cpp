#include "ClientHandler.h"

namespace RingSwarm::proto {
    ClientHandler::ClientHandler(transport::Transport *transport, core::Node *remote) :
            transport(new TransportClientSideWrapper(transport)),
            remote(remote) {
        handshake();
    }

    core::Node *ClientHandler::getRemote() {
        return remote;
    }
}