#ifndef RINGSWARM_SERVERHANDLER_H
#define RINGSWARM_SERVERHANDLER_H

#include "../transport/Buffer.h"
#include "../transport/Transport.h"
#include "RequestHeader.h"
#include "TransportServerSideWrapper.h"
#include <map>
#include <memory>
#include <set>
#include <vector>

namespace RingSwarm::proto {
class ServerHandler {
    RequestHeader nextHeader;
    TransportServerSideWrapper *transport;
    core::Node *remote;

    void listenRequest();

    void errorStop();

    void sendNodeListResponse(std::vector<core::Node *> &nodeList, uint8_t responseType, uint8_t tag);

    void handleHandshake();

    void handleGetKey(transport::Buffer &request, uint8_t tag);

    void handleGetNearestChunk(transport::Buffer &request, uint8_t tag);

    void handleGetChunkLink(transport::Buffer &request, uint8_t tag);

    void handleGetChunk(transport::Buffer &request, uint8_t tag);

    void handleDragIntoKeySwarm(transport::Buffer &request, uint8_t tag);

    void handleNoticeJoinedKeySwarm(transport::Buffer &request, uint8_t tag);

    void handleNoticeJoinedChunkSwarm(transport::Buffer &request, uint8_t tag);

    void handleGetKeySwarm(transport::Buffer &request, uint8_t tag);

    void handleGetChunkSwarm(transport::Buffer &request, uint8_t tag);

    void handleNoticeLeavedChunkSwarm(transport::Buffer &request, uint8_t tag);

    void handleSubscribeOnChunkChange(transport::Buffer &request, uint8_t tag);

    void handleChunkChangeEvent(transport::Buffer &request, uint8_t tag);

    void handleUnsubscribeOnChunkChange(transport::Buffer &request, uint8_t tag);

    typedef void (ServerHandler::*RequestHandler)(transport::Buffer &buffer, uint8_t);

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
            &ServerHandler::handleUnsubscribeOnChunkChange};

    explicit ServerHandler(transport::Transport *transport);

public:
    constexpr static uint16_t MethodsCount = 14;
    static_assert(sizeof(Methods) == sizeof(RequestHandler) * MethodsCount);
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
            "UnsubscribeOnChunkChange"};

    static void Handle(transport::Transport *serverSide);
};
}// namespace RingSwarm::proto

#endif//RINGSWARM_SERVERHANDLER_H
