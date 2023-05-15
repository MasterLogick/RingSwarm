#include "../ServerHandler.h"
#include "../../storage/KeySwarmStorage.h"
#include "../ClientHandler.h"

#define MAX_RESPONSE_SIZE (256 * 8 * 1024)
namespace RingSwarm::proto {
    std::vector<core::Node *> ClientHandler::getKeySwarm(core::Id *keyId) {
        transport::RequestBuffer req(32);
        req.write(keyId);
        transport->sendRequest(8, req);
        auto resp = transport->readResponse(MAX_RESPONSE_SIZE);
        return resp.read<std::vector<core::Node *>>();
    }

    void ServerHandler::handleGetKeySwarm(transport::Buffer &request) {
        auto keyId = request.read<core::Id *>();
        auto keySwarm = storage::getKeySwarm(keyId);
        if (keySwarm == nullptr) {
            transport->sendError();
        } else {
            auto &nodeList = keySwarm->swarm;
            //todo fix
//            sendNodeListResponse(nodeList);
        }
    }
}