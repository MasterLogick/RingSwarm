#include "../ServerHandler.h"
#include "../../storage/FileSwarmStorage.h"
#include "../ClientHandler.h"
#include "../../storage/NodeStorage.h"
#include "../../core/ConnectionManager.h"

#define MAX_RESPONSE_LENGTH (1024 * 32)

namespace RingSwarm::proto {
    bool ClientHandler::getFileMeta(core::Id *fileId, uint8_t nodeIndex, core::FileMeta **meta, core::Node **node) {
        transport::RequestBuffer req(33);
        req.write(fileId);
        req.write<uint8_t>(nodeIndex);
        transport->sendRequest(1, req);
        transport::Buffer resp = transport->readResponse(MAX_RESPONSE_LENGTH);
        auto type = resp.read<uint8_t>();
        if (type == 0) {
            *node = resp.read<core::Node *>();
            return false;
        } else if (type == 1) {
            *meta = resp.read<core::FileMeta *>();
            return true;
        } else {
            throw ProtocolException();
        }
    }

    void ServerHandler::handleGetFileMeta(transport::Buffer &request) {
        auto fileId = request.read<core::Id *>();
        auto index = request.read<uint8_t>();
        auto *fileSwarm = storage::getFileMetaSwarm(fileId);
        if (fileSwarm == nullptr) {
            auto *node = core::getPossibleFileMetaHost(fileId, index)->getRemote();
            if (node == nullptr) {
                transport->sendError();
            }
            transport::ResponseBuffer resp(1 + node->getSerializedSize());
            resp.write<uint8_t>(0);
            resp.write(node);
            transport->sendResponse(resp);
        } else {
            auto *fileMeta = fileSwarm->meta;
            transport::ResponseBuffer resp(1 + fileMeta->getSerializedSize());
            resp.write<uint8_t>(1);
            resp.write(fileMeta);
            transport->sendResponse(resp);
        }
    }

}