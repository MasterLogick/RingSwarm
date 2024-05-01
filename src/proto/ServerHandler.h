#ifndef RINGSWARM_SERVERHANDLER_H
#define RINGSWARM_SERVERHANDLER_H

#include "../transport/Buffer.h"
#include "../transport/Transport.h"

#include "RequestHeader.h"
#include "ServerResponseState.h"

#include <map>
#include <memory>
#include <set>
#include <vector>

namespace RingSwarm::proto {
class ServerHandler {
    std::vector<std::reference_wrapper<ServerResponseState>> finishedHandlers;
    std::map<uint16_t, ServerResponseState> pendingResponses;
    std::unique_ptr<transport::Transport> transport;
    core::Node &remote;
    std::mutex lock;
    bool listening;

    async::Coroutine<> handleRequest(ServerResponseState &serverRespState);

    async::Coroutine<> stubHandler(ServerResponseState &serverRespState);

    /*    async::Coroutine<> handleGetKey(ServerResponseState &request);

    async::Coroutine<> handleGetNearestChunk(ServerResponseState &request);

    async::Coroutine<> handleGetChunkLink(ServerResponseState &request);

    async::Coroutine<> handleGetChunk(ServerResponseState &request);

    async::Coroutine<> handleDragIntoKeySwarm(ServerResponseState &request);

    async::Coroutine<> handleNoticeJoinedKeySwarm(ServerResponseState &request);

    async::Coroutine<> handleNoticeJoinedChunkSwarm(ServerResponseState
    &request);

    async::Coroutine<> handleGetKeySwarm(ServerResponseState &request);

    async::Coroutine<> handleGetChunkSwarm(ServerResponseState &request);

    async::Coroutine<> handleNoticeLeavedChunkSwarm(ServerResponseState
    &request);

    async::Coroutine<> handleSubscribeOnChunkChange(ServerResponseState
    &request);

    async::Coroutine<> handleChunkChangeEvent(ServerResponseState &request);

    async::Coroutine<> handleUnsubscribeOnChunkChange(ServerResponseState
    &request);*/

    typedef async::Coroutine<> (ServerHandler::*
                                    RequestHandler)(ServerResponseState &);

    constexpr static RequestHandler Methods[] = {
        &ServerHandler::stubHandler,// end connection handle
        &ServerHandler::stubHandler,
        &ServerHandler::stubHandler,
        &ServerHandler::stubHandler,
        &ServerHandler::stubHandler,
        &ServerHandler::stubHandler,
        &ServerHandler::stubHandler,
        &ServerHandler::stubHandler,
        &ServerHandler::stubHandler,
        &ServerHandler::stubHandler,
        &ServerHandler::stubHandler,
        &ServerHandler::stubHandler,
        &ServerHandler::stubHandler,
        &ServerHandler::stubHandler,
            /*          &ServerHandler::handleGetKey,
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
&ServerHandler::handleUnsubscribeOnChunkChange*/
    };

    static_assert(
        sizeof(Methods) == sizeof(Methods[0]) * CommandId_COMMAND_COUNT,
        "command handlers count mismatch with command id count"
    );

public:
    explicit ServerHandler(
        std::unique_ptr<transport::Transport> transport,
        core::Node &remote
    );
    async::Coroutine<> listen();
};
}// namespace RingSwarm::proto

#endif// RINGSWARM_SERVERHANDLER_H
