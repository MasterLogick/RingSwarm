#include "ClientHandler.h"
#include "../transport/SecureOverlayTransport.h"

namespace RingSwarm::proto {
    ClientHandler::ClientHandler(transport::Transport *transport, core::Node *remote) :
            transport(new transport::SecureOverlayTransport(transport, remote->publicKey)),
            remote(remote) {
        handshake();
    }
}