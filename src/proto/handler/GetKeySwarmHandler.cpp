#include "../../storage/KeySwarmStorage.h"
#include "../ClientHandler.h"
#include "../ServerHandler.h"

#define MAX_RESPONSE_SIZE (256 * 8 * 1024)
namespace RingSwarm::proto {
std::shared_ptr<async::Future<std::map<uint8_t, core::Node *>>> ClientHandler::getKeySwarm(core::Id *keyId) {
    RequestBuffer req(32);
    req.write(keyId);
    auto f = async::Future<std::map<uint8_t, core::Node *>>::create();
    transport->sendSmallRequest(8, req, MAX_RESPONSE_SIZE)->then([f](auto resp) {
        f->resolve(resp->template readMap<uint8_t, core::Node *>());
    });
    return f;
}

void ServerHandler::handleGetKeySwarm(transport::Buffer &request, uint8_t tag) {
    auto keyId = request.read<core::Id *>();
    auto keySwarm = storage::getKeySwarm(keyId);
    if (keySwarm == nullptr) {
        transport->sendError(tag);
    } else {
        auto &nodeList = keySwarm->swarm;
        uint32_t size = 0;
        for (const auto &item: nodeList) {
            size += item.second->getSerializedSize() + 1;
        }
        auto resp = std::make_shared<ResponseBuffer>(4 + size);
        resp->write(nodeList);
        transport->scheduleResponse(std::move(resp), 1, tag);
    }
}
}// namespace RingSwarm::proto