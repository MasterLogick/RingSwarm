#ifndef RINGSWARM_CLIENTHANDLER_H
#define RINGSWARM_CLIENTHANDLER_H

#include "../core/ChunkLink.h"
#include "../core/Node.h"
#include "../transport/Transport.h"
#include "ClientTransport.h"

namespace RingSwarm::proto {
class ClientHandler {
    ClientTransport clientSideWrapper;

    core::Node &remote;

public:
    ClientHandler(std::unique_ptr<transport::Transport> transport, core::Node &remote);

    async::Coroutine<core::PublicKey *, core::Node *, bool> getKey(
            core::Id *resp, uint8_t nodeIndex);

    async::Coroutine<core::Node *, uint64_t> getNearestChunk(core::Id *keyid, uint64_t chunkIndex);

    async::Coroutine<core::ChunkLink *> getChunkLink(core::Id *keyId, uint64_t chunkIndex);

    async::Coroutine<uint32_t> getChunk(
            core::Id *keyId, uint64_t chunkIndex, uint64_t offset, void *buffer, uint32_t length);

    async::Coroutine<> dragIntoKeySwarm(
            core::PublicKey *key, uint8_t index, std::map<int, core::Node *> &nodeList);

    async::Coroutine<std::vector<core::Node *>> noticeJoinedKeySwarm(core::Id *keyId, uint8_t index);

    async::Coroutine<std::vector<core::Node *>> noticeJoinedChunkSwarm(
            core::Id *keyId, uint64_t chunkIndex);

    async::Coroutine<std::map<uint8_t, core::Node *>> getKeySwarm(core::Id *keyId);

    async::Coroutine<std::vector<core::Node *>> getChunkSwarm(core::Id *resp, uint64_t chunkIndex);

    async::Coroutine<> noticeLeavedChunkSwarm(core::Id *keyId, uint64_t chunkIndex);

    async::Coroutine<> subscribeOnChunkChange(core::Id *keyId, uint64_t chunkIndex);

    async::Coroutine<> chunkChangeEvent(core::Id *keyId, uint64_t chunkIndex, uint8_t changeType);

    async::Coroutine<> unsubscribeOnChunkChange(core::Id *keyId, uint64_t chunkIndex);

    core::Node &getRemote();
};
}// namespace RingSwarm::proto

#endif//RINGSWARM_CLIENTHANDLER_H
