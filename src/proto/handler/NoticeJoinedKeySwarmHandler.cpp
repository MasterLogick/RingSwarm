#include "../ServerHandler.h"
#include "../../storage/KeySwarmStorage.h"
#include "../ClientHandler.h"

#define MAX_RESPONSE_SIZE (256 * 8 * 1024)
namespace RingSwarm::proto {
    std::shared_ptr<async::Future<std::vector<core::Node *>>> ClientHandler::noticeJoinedKeySwarm(
            core::Id *keyId, uint8_t index) {
        RequestBuffer req(33);
        req.write(keyId);
        req.write<uint8_t>(index);
        return transport->sendShortRequest(sizeof(ResponseHeader), req, MAX_RESPONSE_SIZE)->
                then<std::vector<core::Node *>>([](auto resp) {
            return resp->template readVec<core::Node *>();
        });
    }

    void ServerHandler::handleNoticeJoinedKeySwarm(transport::Buffer &request, uint8_t tag) {
        auto keyId = request.read<core::Id *>();
        auto index = request.read<uint8_t>();
        auto keySwarm = storage::getKeySwarm(keyId);
        if (keySwarm == nullptr) {
            transport->sendError(tag);
        } else {
            auto &swarm = keySwarm->swarm;
            if (std::none_of(swarm.begin(), swarm.end(),
                             [this](auto pair) -> bool { return *pair.second == *remote; })) {
                swarm[index] = remote;
            }
            //todo fix
//            sendNodeListResponse(swarm);
        }
    }
}