#include "ConnectionManager.h"
#include "../crypto/HashCrypto.h"

namespace RingSwarm::core {
    std::map<core::Id *, proto::ClientHandler *, core::Id::Comparator> connections;

    proto::ClientHandler *getOrConnect(core::Node *node) {
        if (connections.contains(node->id)) {
            return connections[node->id];
        }
        transport::Transport *transport = node->connectionInfo->openConnection();
        auto *handler = new proto::ClientHandler(transport, node);
        connections[node->id] = handler;
        return handler;
    }

    proto::ClientHandler *getPossibleFileMetaHost(core::Id *fileId, uint8_t index) {
        struct PossibleFileMetaHost {
            uint8_t fileId[32];
            uint8_t nodeId[32];
            uint8_t index;
        }__attribute__((packed)) host{};
        static_assert(sizeof(PossibleFileMetaHost) == 65);
        memcpy(host.fileId, fileId->hash, 32);
        host.index = index;
        proto::ClientHandler *bestHost = connections.begin()->second;
        core::Id *smallestHashValue = core::Id::getBiggestId();
        for (const auto &item: connections) {
            memcpy(host.nodeId, item.first->hash, 32);
            core::Id *hash = crypto::hashData(reinterpret_cast<const uint8_t *>(&host), sizeof(PossibleFileMetaHost));
            if (*hash < *smallestHashValue) {
                delete smallestHashValue;
                smallestHashValue = hash;
                bestHost = item.second;
            }
        }
        delete smallestHashValue;
        return bestHost;
    }

}