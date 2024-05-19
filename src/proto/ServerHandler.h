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
    std::shared_ptr<transport::Transport> transport;
    core::Node &remote;
    std::mutex handlersKeepingLock;
    bool writeLock = false;
    std::vector<std::coroutine_handle<async::Promise<>>> writeLockWaiters;
    std::mutex writeLockWaitersLock;
    bool listening;

    async::Coroutine<> handleRequest(ServerResponseState &serverRespState);

    async::Coroutine<> stubHandler(ServerResponseState &serverRespState);

    async::Coroutine<> handlePing(ServerResponseState &serverRespState);

    async::Coroutine<> handleGetNode(ServerResponseState &serverRespState);

    async::Coroutine<> acquireWriteLock();

    typedef async::Coroutine<> (ServerHandler::*
                                    RequestHandler)(ServerResponseState &);

    constexpr static RequestHandler Methods[] = {
        &ServerHandler::stubHandler, // end connection handle
        &ServerHandler::handlePing,
        &ServerHandler::handleGetNode,
    };

    static_assert(
        sizeof(Methods) == sizeof(Methods[0]) * CommandId_COMMAND_COUNT,
        "command handlers count mismatch with command id count"
    );

public:
    explicit ServerHandler(
        std::shared_ptr<transport::Transport> transport,
        core::Node &remote
    );
    async::Coroutine<> listen();
};
}// namespace RingSwarm::proto

#endif// RINGSWARM_SERVERHANDLER_H
