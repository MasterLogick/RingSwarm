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
        core::ChunkLink *zeroChunk;
        core::KeyInfo keyInfo;

        std::map<uint64_t, uint8_t *> data;
    public:

        ExternalKeyHandler(core::Id *keyId, core::PublicKey *key, proto::ClientHandler *possibleKeySwarmNode);

        std::shared_ptr<async::Future<void *, uint32_t>> readData(uint32_t len, uint64_t offset) override;

        uint64_t getDataSize() override;

        ~ExternalKeyHandler() override;
    };
}

#endif //RINGSWARM_EXTERNALKEYHANDLER_H
