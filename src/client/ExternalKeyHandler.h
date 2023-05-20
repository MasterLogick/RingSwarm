#ifndef RINGSWARM_EXTERNALKEYHANDLER_H
#define RINGSWARM_EXTERNALKEYHANDLER_H

#include "../proto/ClientHandler.h"
#include "../core/KeyInfo.h"
#include "KeyHandler.h"

namespace RingSwarm::client {
    class ExternalKeyHandler : public KeyHandler {
        core::Id *keyId;
        core::PublicKey *key;
        proto::ClientHandler *keySwarmNode;
        proto::ClientHandler *zeroChunkNode;
        core::KeyInfo keyInfo;
    public:

        ExternalKeyHandler(core::Id *keyId, core::PublicKey *key, proto::ClientHandler *possibleKeySwarmNode);

        uint32_t readData(void *buff, uint32_t len, uint64_t offset) override;
    };
}

#endif //RINGSWARM_EXTERNALKEYHANDLER_H
