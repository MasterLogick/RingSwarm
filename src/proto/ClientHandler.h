#ifndef RINGSWARM_CLIENTHANDLER_H
#define RINGSWARM_CLIENTHANDLER_H

#include "../transport/Transport.h"
#include "../core/ChunkLink.h"

namespace RingSwarm::proto {
    class ClientHandler {
        transport::Transport *transport;

        core::Node *remote;

        void handshake();

    public:
        ClientHandler(transport::Transport *transport, core::Node *remote);

        bool getKey(core::Id *keyId, uint8_t nodeIndex, core::PublicKey **key, core::Node **node);

        uint64_t getNearestChunk(core::Id *keyid, uint64_t chunkIndex, core::Node **node);

        core::ChunkLink *getChunkLink(core::Id *keyId, uint64_t chunkIndex);

        uint32_t getChunk(core::Id *keyId, uint64_t chunkIndex, uint64_t offset, void *buffer, uint32_t length);

        void dragIntoKeySwarm(
                core::PublicKey *key,
                uint8_t index,
                std::map<int, core::Node *> &nodeList);

        std::vector<core::Node *> noticeJoinedKeySwarm(core::Id *keyId, uint8_t index);

        std::vector<core::Node *> noticeJoinedChunkSwarm(core::Id *keyId, uint64_t chunkIndex);

        std::map<uint8_t, core::Node *> getKeySwarm(core::Id *keyId);

        std::vector<core::Node *> getChunkSwarm(core::Id *keyId, uint64_t chunkIndex);

        void noticeLeavedChunkSwarm(core::Id *keyId, uint64_t chunkIndex);

        void subscribeOnChunkChange(core::Id *keyId, uint64_t chunkIndex);

        void chunkChangeEvent(core::Id *keyId, uint64_t chunkIndex, uint8_t changeType);

        void unsubscribeOnChunkChange(core::Id *keyId, uint64_t chunkIndex);

        core::Node *getRemote();
    };
}

#endif //RINGSWARM_CLIENTHANDLER_H
