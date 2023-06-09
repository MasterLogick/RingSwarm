#include "ConnectionManager.h"
#include "../crypto/HashCrypto.h"
#include "../transport/SecureOverlayTransport.h"

namespace RingSwarm::core {
    std::map<core::Id *, proto::ClientHandler *, core::Id::Comparator> connections;

    std::shared_ptr<async::Future<proto::ClientHandler *>> getOrConnect(core::Node *node) {
        if (connections.contains(node->id)) {
            return async::Future<proto::ClientHandler *>::createResolved(connections[node->id]);
        }
        auto *transport = node->connectionInfo->openConnection();
        return transport::SecureOverlayTransport::createClientSide(transport, node->publicKey)->
                then<proto::ClientHandler *>([node](auto *overlay) {
            return (connections[node->id] = new proto::ClientHandler(overlay, node));
        });
    }

    proto::ClientHandler *getPossibleKeyHost(core::Id *keyId, uint8_t index) {
        struct PossibleKeyHost {
            uint8_t keyHash[32];
            uint8_t nodeHash[32];
            uint8_t index;
        }__attribute__((packed)) host{};
        static_assert(sizeof(PossibleKeyHost) == 65);
        memcpy(host.keyHash, keyId->hash, 32);
        host.index = index;
        proto::ClientHandler *bestHost = connections.begin()->second;
        core::Id *smallestHashValue = core::Id::getBiggestId();
        for (const auto &item: connections) {
            memcpy(host.nodeHash, item.first->hash, 32);
            core::Id *hash = crypto::hashData(&host, sizeof(PossibleKeyHost));
            if (*hash < *smallestHashValue) {
                delete smallestHashValue;
                smallestHashValue = hash;
                bestHost = item.second;
            }
        }
        delete smallestHashValue;
        return bestHost;
    }

    std::shared_ptr<async::Future<proto::ClientHandler *>> getOrConnectToOne(std::vector<core::Node *> &nodeList) {
        std::vector<core::Node *> copy(nodeList);
        //todo shuffle
        //todo
        for (const auto &item: copy) {
            try {
                auto client = core::getOrConnect(item);
                return client;
            } catch (std::exception &) {}
        }
        return nullptr;
    }

}