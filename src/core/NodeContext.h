#ifndef RINGSWARM_SRC_CORE_NODECONTEXT_H
#define RINGSWARM_SRC_CORE_NODECONTEXT_H

#include "../async/coroutine.h"
#include "../transport/PlainSocketServer.h"

#include "Node.h"

namespace RingSwarm::core {
class NodeContext {
    Node node;
    std::vector<async::Coroutine<>> serverHandlers;
    std::vector<std::unique_ptr<transport::PlainSocketServer>> servers;

    async::Coroutine<> handleServerConnection(
        std::unique_ptr<transport::PlainSocketTransport> serverSideSocket
    );

public:
    ~NodeContext();
    int addServer(std::string serverSideSocket, int port);
};
}// namespace RingSwarm::core

#endif// RINGSWARM_SRC_CORE_NODECONTEXT_H
