#include "../ServerHandler.h"
#include "../../storage/KeySwarmStorage.h"
#include "../ClientHandler.h"
#include "../../core/ConnectionManager.h"

#define MAX_RESPONSE_LENGTH (1024 * 32)

namespace RingSwarm::proto {
    std::shared_ptr<async::Future<core::PublicKey *, core::Node *, bool>>
    ClientHandler::getKey(core::Id *keyId, uint8_t nodeIndex) {
        RequestBuffer req(33);
        req.write(keyId);
        req.write<uint8_t>(nodeIndex);
        auto f = async::Future<core::PublicKey *, core::Node *, bool>::create();
        transport->sendRequest(1, req, MAX_RESPONSE_LENGTH)->then([this, f](ResponseHeader header) {
            transport->readBuffer(header.responseLen)->then([header, f](auto resp) {
                if (header.responseType == 1) {
                    auto *node = resp->template read<core::Node *>();
                    f->resolve(nullptr, node, true);
                } else if (header.responseType == 2) {
                    auto *key = resp->template read<core::PublicKey *>();
                    f->resolve(key, nullptr, true);
                } else {
                    throw ProtocolException();
                }
            });
        });
        return f;
    }

    void ServerHandler::handleGetKey(transport::Buffer &request, uint8_t tag) {
        auto keyId = request.read<core::Id *>();
        auto index = request.read<uint8_t>();
        auto *keySwarm = storage::getKeySwarm(keyId);
        if (keySwarm == nullptr) {
            auto *client = core::getPossibleKeyHost(keyId, index);
            if (client == nullptr) {
                transport->sendError(tag);
                return;
            }
            auto *node = client->getRemote();
            if (node == nullptr) {
                transport->sendError(tag);
                return;
            }
            auto resp = std::make_shared<ResponseBuffer>(1 + node->getSerializedSize());
            resp->write<uint8_t>(0);
            resp->write(node);
            transport->scheduleResponse(std::move(resp), 1, tag);
        } else {
            auto *key = keySwarm->key;
            auto resp = std::make_shared<ResponseBuffer>(1 + key->size());
            resp->write<uint8_t>(1);
            resp->write(key);
            transport->scheduleResponse(std::move(resp), 2, tag);
        }
    }

}