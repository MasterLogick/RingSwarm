#include "../ServerHandler.h"
#include "../../storage/FileSwarmStorage.h"
#include "../ClientHandler.h"

#define MAX_RESPONSE_SIZE (0)
namespace RingSwarm::proto {
    void ClientHandler::dragIntoFileMetaSwarm(
            core::FileMeta *meta,
            uint8_t index,
            std::map<int, core::Node *> &nodeList
    ) {
        uint32_t size = 0;
        for (const auto &item: nodeList) {
            size += item.second->getSerializedSize();
        }
        transport::RequestBuffer req(32 + 1 + 1 + nodeList.size() + size);
        req.write(meta);
        req.write<uint8_t>(index);
        req.write<uint8_t>(size);
        std::vector<core::Node *> nodes;
        for (const auto &item: nodeList) {
            req.write<uint8_t>(item.first);
            nodes.push_back(item.second);
        }
        req.write<std::vector<core::Node *> &>(nodes);
        transport->sendRequest(5, req);
        transport->readResponse(MAX_RESPONSE_SIZE);
    }

    void ServerHandler::handleDragIntoFileMetaSwarm(transport::Buffer &request) {
        core::FileMeta *meta = request.read<core::FileMeta *>();
        auto index = request.read<uint8_t>();
        auto swarmSize = request.read<uint8_t>();
        uint8_t swarmIndexes[swarmSize];
        for (int i = 0; i < swarmSize; ++i) {
            swarmIndexes[i] = request.read<uint8_t>();
        }
        //todo read node indexes
        auto nodeList = request.read<std::vector<core::Node *>>();
        auto swarm = storage::getFileMetaSwarm(meta->fileId);
        if (swarm != nullptr) {
            //todo update node list
            transport->sendEmptyResponse();
        } else {

            auto *newSwarm = new core::FileMetaSwarm();
            storage::storeFileMetaSwarm(newSwarm);
            //todo handle file meta propagation
            transport->sendEmptyResponse();
        }
    }
}