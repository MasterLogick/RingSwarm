#include "ConnectionManager.h"

namespace RingSwarm::core {
    std::map<core::Id *, proto::ClientHandler *, core::Id::Comparator> connections;

    proto::ClientHandler *getOrConnect(core::Node *node) {
        if (connections.contains(node->id)) {
            return connections[node->id];
        }
        transport::Transport *transport = node->connectionInfo->openConnection();
        auto *handler = new proto::ClientHandler(transport,node);
        connections[node->id] = handler;
        return handler;
    }

    core::Node *getPossibleFileMetaHost(core::Id *, uint8_t index) {
        return nullptr;
    }

}