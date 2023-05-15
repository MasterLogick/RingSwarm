#include "../ServerHandler.h"
#include "../../storage/KeySwarmStorage.h"
#include "../ClientHandler.h"
#include "../../storage/NodeStorage.h"
#include "../../core/ConnectionManager.h"

#define MAX_RESPONSE_LENGTH (1024 * 32)

namespace RingSwarm::proto {
    bool ClientHandler::getKey(core::Id *keyId, uint8_t nodeIndex, core::PublicKey **key, core::Node **node) {
        transport::RequestBuffer req(33);
        req.write(keyId);
        req.write<uint8_t>(nodeIndex);
        transport->sendRequest(1, req);
        transport::Buffer resp = transport->readResponse(MAX_RESPONSE_LENGTH);
        auto type = resp.read<uint8_t>();
        if (type == 0) {
            *node = resp.read<core::Node *>();
            return false;
        } else if (type == 1) {
            *key = resp.read<core::PublicKey *>();
            return true;
        } else {
            throw ProtocolException();
        }
    }

    void ServerHandler::handleGetKey(transport::Buffer &request) {
        auto keyId = request.read<core::Id *>();
        auto index = request.read<uint8_t>();
        auto *keySwarm = storage::getKeySwarm(keyId);
        if (keySwarm == nullptr) {
            auto *client = core::getPossibleKeyHost(keyId, index);
            if (client == nullptr) {
                transport->sendError();
                return;
            }
            auto *node = client->getRemote();
            if (node == nullptr) {
                transport->sendError();
                return;
            }
            transport::ResponseBuffer resp(1 + node->getSerializedSize());
            resp.write<uint8_t>(0);
            resp.write(node);
            transport->sendResponse(resp);
        } else {
            auto *key = keySwarm->key;
            transport::ResponseBuffer resp(1 + key->size());
            resp.write<uint8_t>(1);
            resp.write(key);
            transport->sendResponse(resp);
        }
    }

}