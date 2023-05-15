#include "../ServerHandler.h"
#include "../../storage/KeySwarmStorage.h"
#include "../ClientHandler.h"

#define MAX_RESPONSE_SIZE (256 * 8 * 1024)
namespace RingSwarm::proto {
    std::vector<core::Node *> ClientHandler::noticeJoinedKeySwarm(core::Id *keyId, uint8_t index) {
        transport::RequestBuffer req(33);
        req.write(keyId);
        req.write<uint8_t>(index);
        transport->sendRequest(6, req);
        auto resp = transport->readResponse(MAX_RESPONSE_SIZE);
        return resp.read<std::vector<core::Node *>>();
    }

    void ServerHandler::handleNoticeJoinedKeySwarm(transport::Buffer &request) {
        auto keyId = request.read<core::Id *>();
        auto index = request.read<uint8_t>();
        auto keySwarm = storage::getKeySwarm(keyId);
        if (keySwarm == nullptr) {
            transport->sendError();
        } else {
            auto &swarm = keySwarm->swarm;
            if (std::none_of(swarm.begin(), swarm.end(),
                             [&](auto pair) -> bool { return *pair.second == *remote; })) {
                swarm[index] = remote;
            }
            //todo fix
//            sendNodeListResponse(swarm);
        }
    }
}