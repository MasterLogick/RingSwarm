#ifndef RINGSWARM_SERVERHANDLER_H
#define RINGSWARM_SERVERHANDLER_H

#include <map>
#include <vector>
#include <memory>
#include <boost/asio/awaitable.hpp>
#include <boost/asio/ip/tcp.hpp>
#include "../transport/Transport.h"

namespace RingSwarm::proto {
    class ServerHandler {
        transport::Transport *transport;
        boost::asio::ip::tcp::socket sock;
        std::shared_ptr<core::Node> remote;

        void errorStop();

        boost::asio::awaitable<void> sendNodeListResponse(std::vector<std::shared_ptr<core::Node>> &nodeList);

        void handleHandshake();

        boost::asio::awaitable<void> handleGetFileMeta(transport::Buffer &request);

        boost::asio::awaitable<void> handleGetNearestChunk(transport::Buffer &request);

        boost::asio::awaitable<void> handleGetChunkLink(transport::Buffer &request);

        boost::asio::awaitable<void> handleGetChunk(transport::Buffer &request);

        boost::asio::awaitable<void> handleDragIntoFileMetaSwarm(transport::Buffer &request);

        boost::asio::awaitable<void> handleNoticeJoinedFileMetaSwarm(transport::Buffer &request);

        boost::asio::awaitable<void> handleNoticeJoinedChunkSwarm(transport::Buffer &request);

        boost::asio::awaitable<void> handleGetFileMetaSwarm(transport::Buffer &request);

        boost::asio::awaitable<void> handleGetChunkSwarm(transport::Buffer &request);

        boost::asio::awaitable<void> handleNoticeLeavedChunkSwarm(transport::Buffer &request);

        boost::asio::awaitable<void> handleSubscribeOnChunkChange(transport::Buffer &request);

        boost::asio::awaitable<void> handleChunkChangeEvent(transport::Buffer &request);

        boost::asio::awaitable<void> handleUnsubscribeOnChunkChange(transport::Buffer &request);

        typedef boost::asio::awaitable<void> (ServerHandler::*RequestHandler)(transport::Buffer &buffer);

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

        explicit ServerHandler(boost::asio::ip::tcp::socket &&socket);

        boost::asio::awaitable<void> handleClientConnection();

        void sendError();
    };
}

#endif //RINGSWARM_SERVERHANDLER_H
