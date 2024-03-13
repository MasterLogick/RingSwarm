#ifndef RINGSWARM_CLIENTHANDLER_H
#define RINGSWARM_CLIENTHANDLER_H

#include "../core/ChunkLink.h"
#include "../core/Node.h"
#include "../transport/Transport.h"
#include "TransportClientSideWrapper.h"

namespace RingSwarm::proto {
class ClientHandler {
    TransportClientSideWrapper *transport;

    core::Node *remote;

    void handshake();

public:
    ClientHandler(transport::Transport *transport, core::Node *remote);

    std::shared_ptr<async::Future<core::PublicKey *, core::Node *, bool>> getKey(
            core::Id *resp, uint8_t nodeIndex);

    std::shared_ptr<async::Future<core::Node *, uint64_t>> getNearestChunk(core::Id *keyid, uint64_t chunkIndex);

    std::shared_ptr<async::Future<core::ChunkLink *>> getChunkLink(core::Id *keyId, uint64_t chunkIndex);

    std::shared_ptr<async::Future<uint32_t>> getChunk(
            core::Id *keyId, uint64_t chunkIndex, uint64_t offset, void *buffer, uint32_t length);

    std::shared_ptr<async::Future<void>> dragIntoKeySwarm(
            core::PublicKey *key, uint8_t index, std::map<int, core::Node *> &nodeList);

    std::shared_ptr<async::Future<std::vector<core::Node *>>> noticeJoinedKeySwarm(core::Id *keyId, uint8_t index);

    std::shared_ptr<async::Future<std::vector<core::Node *>>> noticeJoinedChunkSwarm(
            core::Id *keyId, uint64_t chunkIndex);

    std::shared_ptr<async::Future<std::map<uint8_t, core::Node *>>> getKeySwarm(core::Id *keyId);

    std::shared_ptr<async::Future<std::vector<core::Node *>>> getChunkSwarm(core::Id *resp, uint64_t chunkIndex);

    std::shared_ptr<async::Future<void>> noticeLeavedChunkSwarm(core::Id *keyId, uint64_t chunkIndex);

    std::shared_ptr<async::Future<void>> subscribeOnChunkChange(core::Id *keyId, uint64_t chunkIndex);

    std::shared_ptr<async::Future<void>> chunkChangeEvent(core::Id *keyId, uint64_t chunkIndex, uint8_t changeType);

    std::shared_ptr<async::Future<void>> unsubscribeOnChunkChange(core::Id *keyId, uint64_t chunkIndex);

    core::Node *getRemote();
};
}// namespace RingSwarm::proto

#endif//RINGSWARM_CLIENTHANDLER_H
