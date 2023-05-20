#include "../ServerHandler.h"
#include "../../storage/KeySwarmStorage.h"
#include "../ClientHandler.h"

#define MAX_RESPONSE_SIZE (0)
namespace RingSwarm::proto {
    void ClientHandler::dragIntoKeySwarm(
            core::PublicKey *key,
            uint8_t index,
            std::map<int, core::Node *> &nodeList
    ) {
        uint32_t size = 0;
        for (const auto &item: nodeList) {
            size += item.second->getSerializedSize();
        }
        transport::RequestBuffer req(32 + 1 + 1 + nodeList.size() + size);
        req.write(key);
        req.write<uint8_t>(index);
        req.write<uint8_t>(size);
        std::vector<core::Node *> nodes;
        for (const auto &item: nodeList) {
            req.write<uint8_t>(item.first);
            nodes.push_back(item.second);
        }
        req.write<core::Node *>(nodes);
        transport->sendRequest(5, req);
        transport->readResponse(MAX_RESPONSE_SIZE);
    }

    void ServerHandler::handleDragIntoKeySwarm(transport::Buffer &request) {
        core::PublicKey *key = request.read<core::PublicKey *>();
        auto index = request.read<uint8_t>();
        auto swarmSize = request.read<uint8_t>();
        uint8_t swarmIndexes[swarmSize];
        for (int i = 0; i < swarmSize; ++i) {
            swarmIndexes[i] = request.read<uint8_t>();
        }
        //todo read node indexes
        auto nodeList = request.readVec<core::Node *>();
        auto *swarm = storage::getKeySwarm(key->getId());
        if (swarm != nullptr) {
            //todo update node list
            transport->sendEmptyResponse();
        } else {

            auto *newSwarm = new core::KeySwarm();
            storage::storeKeySwarm(newSwarm);
            //todo handle keyId key propagation
            transport->sendEmptyResponse();
        }
    }
}