#include "../ServerHandler.h"
#include "../../storage/FileSwarmStorage.h"
#include "../ClientHandler.h"

#define MAX_RESPONSE_SIZE (256 * 8 * 1024)
namespace RingSwarm::proto {
    std::vector<core::Node *> ClientHandler::getFileMetaSwarm(core::Id &fileId) {
        transport::RequestBuffer req(32);
        req.writeId(fileId);
        transport->sendRequest(8, req);
        auto resp = transport->readResponse(MAX_RESPONSE_SIZE);
        return resp.readNodeList();
    }

    void ServerHandler::handleGetFileMetaSwarm(transport::Buffer &request) {
        auto id = request.readId();
        auto fileMetaSwarm = storage::getHostedFileMetaSwarm(id);
        if (fileMetaSwarm == nullptr) {
            transport->sendError();
        } else {
            auto nodeList = fileMetaSwarm->getSwarmNodes();
            sendNodeListResponse(nodeList);
        }
    }
}