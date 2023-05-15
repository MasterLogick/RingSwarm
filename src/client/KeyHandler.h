#ifndef RINGSWARM_KEYHANDLER_H
#define RINGSWARM_KEYHANDLER_H

#include "../proto/ClientHandler.h"

namespace RingSwarm::client {
    class KeyHandler {
        core::PublicKey *key;
        proto::ClientHandler *possibleKeySwarmNode;
    public:
        KeyHandler(core::PublicKey *key, proto::ClientHandler *possibleKeySwarmNode) :
                key(key), possibleKeySwarmNode(possibleKeySwarmNode) {

        }
    };
}

#endif //RINGSWARM_KEYHANDLER_H
