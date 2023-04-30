#ifndef RINGSWARM_CLIENTHANDLER_H
#define RINGSWARM_CLIENTHANDLER_H

#include "../transport/Transport.h"
#include "../core/ChunkLink.h"

namespace RingSwarm::proto {
    class ClientHandler {
        transport::Transport *transport;

        void handshake();

    public:
        explicit ClientHandler(transport::Transport *transport);

        bool getFileMeta(core::Id &fileId, uint8_t nodeIndex, core::FileMeta **meta, core::Node **node);

        uint64_t getNearestChunk(core::Id &fileId, uint64_t chunkIndex, core::Node **node);

        core::ChunkLink *getChunkLink(core::Id &id, uint64_t chunkIndex);

        void getChunk(core::Id &fileId, uint64_t chunkIndex, uint32_t offset, void *buffer, uint32_t length);

        void dragIntoFileMetaSwarm(
                core::FileMeta *meta,
                uint8_t index,
                std::vector<std::pair<std::shared_ptr<core::Node>, uint8_t>> &nodeList);

        std::vector<core::Node *> noticeJoinedFileMetaSwarm(core::Id &fileId);

        std::vector<core::Node *> noticeJoinedChunkSwarm(core::Id &fileId, uint64_t chunkIndex);

        std::vector<core::Node *> getFileMetaSwarm(core::Id &fileId);

        std::vector<core::Node *> getChunkSwarm(core::Id &fileId, uint64_t chunkIndex);

        void noticeLeavedChunkSwarm(core::Id &fileId, uint64_t chunkIndex);

        void subscribeOnChunkChange(core::Id &fileId, uint64_t chunkIndex);

        void chunkChangeEvent(core::Id &fileId, uint64_t chunkIndex, uint8_t changeType);

        void unsubscribeOnChunkChange(core::Id &fileId, uint64_t chunkIndex);
    };
}

#endif //RINGSWARM_CLIENTHANDLER_H
