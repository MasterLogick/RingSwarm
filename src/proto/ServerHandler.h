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

        void handleGetKey(transport::Buffer &request);

        void handleGetNearestChunk(transport::Buffer &request);

        void handleGetChunkLink(transport::Buffer &request);

        void handleGetChunk(transport::Buffer &request);

        void handleDragIntoKeySwarm(transport::Buffer &request);

        void handleNoticeJoinedKeySwarm(transport::Buffer &request);

        void handleNoticeJoinedChunkSwarm(transport::Buffer &request);

        void handleGetKeySwarm(transport::Buffer &request);

        void handleGetChunkSwarm(transport::Buffer &request);

        void handleNoticeLeavedChunkSwarm(transport::Buffer &request);

        void handleSubscribeOnChunkChange(transport::Buffer &request);

        void handleChunkChangeEvent(transport::Buffer &request);

        void handleUnsubscribeOnChunkChange(transport::Buffer &request);

        typedef void (ServerHandler::*RequestHandler)(transport::Buffer &buffer);

        constexpr static RequestHandler Methods[] = {
                nullptr,
                &ServerHandler::handleGetKey,
                &ServerHandler::handleGetNearestChunk,
                &ServerHandler::handleGetChunkLink,
                &ServerHandler::handleGetChunk,
                &ServerHandler::handleDragIntoKeySwarm,
                &ServerHandler::handleNoticeJoinedKeySwarm,
                &ServerHandler::handleNoticeJoinedChunkSwarm,
                &ServerHandler::handleGetKeySwarm,
                &ServerHandler::handleGetChunkSwarm,
                &ServerHandler::handleNoticeLeavedChunkSwarm,
                &ServerHandler::handleSubscribeOnChunkChange,
                &ServerHandler::handleChunkChangeEvent,
                &ServerHandler::handleUnsubscribeOnChunkChange
        };
        constexpr static uint16_t MethodsCount = 14;
        static_assert(sizeof(Methods) == sizeof(RequestHandler) * MethodsCount);
    public:
        constexpr static const char *MethodNames[] = {
                nullptr,
                "GetKey",
                "GetNearestChunk",
                "GetChunkLink",
                "GetChunk",
                "DragIntoKeySwarm",
                "NoticeJoinedKeySwarm",
                "NoticeJoinedChunkSwarm",
                "GetKeySwarm",
                "GetChunkSwarm",
                "NoticeLeavedChunkSwarm",
                "SubscribeOnChunkChange",
                "ChunkChangeEvent",
                "UnsubscribeOnChunkChange"
        };

        explicit ServerHandler(transport::Transport *transport);

        void handleClientConnection();
    };
}

#endif //RINGSWARM_SERVERHANDLER_H
