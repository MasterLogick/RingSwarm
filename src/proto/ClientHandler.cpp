#include "ClientHandler.h"

namespace RingSwarm::proto {
    ClientHandler::ClientHandler(transport::Transport *transport) : transport(transport) {
        handshake();
    }
}