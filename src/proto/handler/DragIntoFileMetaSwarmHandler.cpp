#include "../ServerHandler.h"
#include "../../storage/FileSwarmStorage.h"
#include "../ClientHandler.h"

#define MAX_RESPONSE_SIZE (0)
namespace RingSwarm::proto {
    void ClientHandler::dragIntoFileMetaSwarm(
            core::FileMeta *meta,
            uint8_t index,
            std::vector<std::pair<std::shared_ptr<core::Node>, uint8_t>> &nodeList
    ) {
        uint32_t size = 0;
        for (const auto &item: nodeList) {
            size += item.first->getSerializedSize();
        }
        transport::RequestBuffer req(32 + 1 + 1 + nodeList.size() + size);
        req.writeFileMeta(meta);
        req.writeUint8(index);
        req.writeUint8(size);
        std::vector<std::shared_ptr<core::Node>> nodes;
        for (const auto &item: nodeList) {
            req.writeUint8(item.second);
            nodes.push_back(item.first);
        }
        req.writeNodeList(nodes);
        transport->sendRequest(5, req);
        transport->readResponse(MAX_RESPONSE_SIZE);
    }

    void ServerHandler::handleDragIntoFileMetaSwarm(transport::Buffer &request) {
        core::FileMeta *meta = request.readFileMeta();
        auto index = request.readUint8();
        auto swarmSize = request.readUint8();
        uint8_t swarmIndexes[swarmSize];
        for (int i = 0; i < swarmSize; ++i) {
            swarmIndexes[i] = request.readUint8();
        }
        //todo read node indexes
        auto nodeList = request.readNodeList();
        auto swarm = storage::getHostedFileMetaSwarm(meta->getId());
        if (swarm != nullptr) {
            //todo update node list
            transport->sendEmptyResponse();
        } else {
            
            std::shared_ptr<core::FileMetaSwarm> newSwarm(new core::FileMetaSwarm(meta, index, nodeList));
            storage::storeNewFileMetaSwarm(newSwarm);
            //todo handle file meta propagation
            transport->sendEmptyResponse();
        }
    }
}