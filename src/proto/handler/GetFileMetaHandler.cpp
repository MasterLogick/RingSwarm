#include "../ServerHandler.h"
#include "../../storage/FileSwarmStorage.h"
#include "../../storage/ConnectionsStorage.h"
#include "../ClientHandler.h"

#define MAX_RESPONSE_LENGTH (1024 * 32)

namespace RingSwarm::proto {
    bool ClientHandler::getFileMeta(core::Id &fileId, uint8_t nodeIndex, core::FileMeta **meta, core::Node **node) {
        transport::RequestBuffer req(33);
        req.writeId(fileId);
        req.writeUint8(nodeIndex);
        transport->sendRequest(1, req);
        transport::Buffer resp = transport->readResponse(MAX_RESPONSE_LENGTH);
        auto type = resp.readUint8();
        if (type == 0) {
            *node = resp.readNode();
            return false;
        } else if (type == 1) {
            *meta = resp.readFileMeta();
            return true;
        } else {
            throw ProtocolException();
        }
    }

    void ServerHandler::handleGetFileMeta(transport::Buffer &request) {
        auto fileId = request.readId();
        auto index = request.readUint8();
        auto fileSwarm = storage::getHostedFileMetaSwarm(fileId);
        if (fileSwarm == nullptr) {
            auto node = storage::getPossibleFileMetaHost(fileId, index);
            transport::ResponseBuffer resp(1 + node->getSerializedSize());
            resp.writeUint8(0);
            resp.writeNode(node);
            transport->sendResponse(resp);
        } else {
            auto *fileMeta = fileSwarm->getFileMeta();
            transport::ResponseBuffer resp(1 + fileMeta->getSerializedSize());
            resp.writeUint8(1);
            resp.writeFileMeta(fileMeta);
            transport->sendResponse(resp);
        }
    }

}