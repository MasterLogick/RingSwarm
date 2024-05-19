#ifndef RINGSWARM_SRC_CORE_NODECONTEXT_H
#define RINGSWARM_SRC_CORE_NODECONTEXT_H

#include "../async/coroutine.h"
#include "../transport/PlainSocketServer.h"
#include "../transport/Transport.h"

#include "Node.h"
#include "ServerHandlerContext.h"

namespace RingSwarm::core {
class NodeContext {
    Node node;
    std::list<ServerHandlerContext> serverHandlers;
    std::list<ServerHandlerContext> finishedHandlers;
    std::mutex serverHandlersMutex;
    std::mutex finishedHandlersMutex;
    std::vector<std::shared_ptr<transport::PlainSocketServer>> servers;

    async::Coroutine<> handleServerConnection(
        std::shared_ptr<transport::Transport> serverSideSocket,
        std::list<ServerHandlerContext>::iterator ref
    );

    void cleanupFinishedHandlers();

public:
    ~NodeContext();
    int addServer(std::string serverSideSocket, int port);
};
}// namespace RingSwarm::core

#endif// RINGSWARM_SRC_CORE_NODECONTEXT_H
