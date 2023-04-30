#ifndef RINGSWARM_SERVERHANDLER_H
#define RINGSWARM_SERVERHANDLER_H

#include <map>
#include <vector>
#include <memory>
#include "../transport/Transport.h"

namespace RingSwarm::proto {
    class ServerHandler {
        transport::Transport *transport;
        core::Node *remote;

        void errorStop();

        void sendNodeListResponse(std::vector<core::Node *> &nodeList);

        void handleHandshake();

        void handleGetFileMeta(transport::Buffer &request);

        void handleGetNearestChunk(transport::Buffer &request);

        void handleGetChunkLink(transport::Buffer &request);

        void handleGetChunk(transport::Buffer &request);

        void handleDragIntoFileMetaSwarm(transport::Buffer &request);

        void handleNoticeJoinedFileMetaSwarm(transport::Buffer &request);

        void handleNoticeJoinedChunkSwarm(transport::Buffer &request);

        void handleGetFileMetaSwarm(transport::Buffer &request);

        void handleGetChunkSwarm(transport::Buffer &request);

        void handleNoticeLeavedChunkSwarm(transport::Buffer &request);

        void handleSubscribeOnChunkChange(transport::Buffer &request);

        void handleChunkChangeEvent(transport::Buffer &request);

        void handleUnsubscribeOnChunkChange(transport::Buffer &request);

        typedef void (ServerHandler::*RequestHandler)(transport::Buffer &buffer);

        constexpr static RequestHandler Methods[] = {
                nullptr,
                &ServerHandler::handleGetFileMeta,
                &ServerHandler::handleGetNearestChunk,
                &ServerHandler::handleGetChunkLink,
                &ServerHandler::handleGetChunk,
                &ServerHandler::handleDragIntoFileMetaSwarm,
                &ServerHandler::handleNoticeJoinedFileMetaSwarm,
                &ServerHandler::handleNoticeJoinedChunkSwarm,
                &ServerHandler::handleGetFileMetaSwarm,
                &ServerHandler::handleGetChunkSwarm,
                &ServerHandler::handleNoticeLeavedChunkSwarm,
                &ServerHandler::handleSubscribeOnChunkChange,
                &ServerHandler::handleChunkChangeEvent,
                &ServerHandler::handleUnsubscribeOnChunkChange
        };
        constexpr static const char *MethodNames[] = {
                nullptr,
                "GetFileMeta",
                "GetNearestChunk",
                "GetChunkLink",
                "GetChunk",
                "DragIntoFileMetaSwarm",
                "NoticeJoinedFileMetaSwarm",
                "NoticeJoinedChunkSwarm",
                "GetFileMetaSwarm",
                "GetChunkSwarm",
                "NoticeLeavedChunkSwarm",
                "SubscribeOnChunkChange",
                "ChunkChangeEvent",
                "UnsubscribeOnChunkChange"
        };

        constexpr static uint16_t MethodsCount = 14;
        static_assert(sizeof(Methods) / sizeof(RequestHandler) == MethodsCount);
    public:

        explicit ServerHandler(transport::Transport *transport);

        void handleClientConnection();
    };
}

#endif //RINGSWARM_SERVERHANDLER_H
