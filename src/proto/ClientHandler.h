#ifndef RINGSWARM_CLIENTHANDLER_H
#define RINGSWARM_CLIENTHANDLER_H

#include "../transport/Transport.h"
#include "../core/ChunkLink.h"

namespace RingSwarm::proto {
    class ClientHandler {
        transport::Transport *transport;

        boost::asio::awaitable<void> handshake();

    public:
        explicit ClientHandler(transport::Transport *transport);

        boost::asio::awaitable<bool>
        getFileMeta(core::Id &fileId, uint8_t nodeIndex, core::FileMeta **meta, core::Node **node);

        boost::asio::awaitable<uint64_t> getNearestChunk(core::Id &fileId, uint64_t chunkIndex, core::Node **node);

        boost::asio::awaitable<core::ChunkLink *> getChunkLink(core::Id &id, uint64_t chunkIndex);

        boost::asio::awaitable<void>
        getChunk(core::Id &fileId, uint64_t chunkIndex, uint32_t offset, void *buffer, uint32_t length);

        boost::asio::awaitable<void> dragIntoFileMetaSwarm(
                core::FileMeta *meta,
                std::vector<std::pair<std::shared_ptr<core::Node>, uint8_t>> &nodeList);

        boost::asio::awaitable<std::vector<core::Node *>> noticeJoinedFileMetaSwarm(core::Id &fileId);

        boost::asio::awaitable<std::vector<core::Node *>> noticeJoinedChunkSwarm(core::Id &fileId, uint64_t chunkIndex);

        boost::asio::awaitable<std::vector<core::Node *>> getFileMetaSwarm(core::Id &fileId);

        boost::asio::awaitable<std::vector<core::Node *>> getChunkSwarm(core::Id &fileId, uint64_t chunkIndex);

        boost::asio::awaitable<void> noticeLeavedChunkSwarm(core::Id &fileId, uint64_t chunkIndex);

        boost::asio::awaitable<void> subscribeOnChunkChange(core::Id &fileId, uint64_t chunkIndex);

        boost::asio::awaitable<void> chunkChangeEvent(core::Id &fileId, uint64_t chunkIndex, uint8_t changeType);

        boost::asio::awaitable<void> unsubscribeOnChunkChange(core::Id &fileId, uint64_t chunkIndex);
    };
}

#endif //RINGSWARM_CLIENTHANDLER_H
