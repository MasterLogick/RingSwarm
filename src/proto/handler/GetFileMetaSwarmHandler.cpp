#include "../ServerHandler.h"
#include "../../storage/FileSwarmStorage.h"
#include "../ClientHandler.h"

#define MAX_RESPONSE_SIZE (256 * 8 * 1024)
namespace RingSwarm::proto {
    std::vector<core::Node *> ClientHandler::getFileMetaSwarm(core::Id *fileId) {
        transport::RequestBuffer req(32);
        req.write(fileId);
        transport->sendRequest(8, req);
        auto resp = transport->readResponse(MAX_RESPONSE_SIZE);
        return resp.read<std::vector<core::Node *>>();
    }

    void ServerHandler::handleGetFileMetaSwarm(transport::Buffer &request) {
        auto id = request.read<core::Id *>();
        auto fileMetaSwarm = storage::getFileMetaSwarm(id);
        if (fileMetaSwarm == nullptr) {
            transport->sendError();
        } else {
            auto &nodeList = fileMetaSwarm->swarm;
            //todo fix
//            sendNodeListResponse(nodeList);
        }
    }
}