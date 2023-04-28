#include "../ServerHandler.h"
#include "../../storage/FileSwarmStorage.h"
#include "../../storage/ConnectionsStorage.h"
#include "../ClientHandler.h"

#define MAX_RESPONSE_LENGTH (1024 * 32)

namespace RingSwarm::proto {
    boost::asio::awaitable<bool>
    ClientHandler::getFileMeta(core::Id &fileId, uint8_t nodeIndex, core::FileMeta **meta, core::Node **node) {
        transport::RequestBuffer req(33);
        req.writeId(fileId);
        req.writeUint8(nodeIndex);
        co_await transport->sendRequest(1, req);
        transport::Buffer resp = co_await transport->readResponse(MAX_RESPONSE_LENGTH);
        auto type = resp.readUint8();
        if (type == 0) {
            *node = resp.readNode();
            co_return false;
        } else if (type == 1) {
            *meta = resp.readFileMeta();
            co_return true;
        } else {
            throw ProtocolException();
        }
    }

    boost::asio::awaitable<void> ServerHandler::handleGetFileMeta(transport::Buffer &request) {
        auto fileId = request.readId();
        auto index = request.readUint8();
        auto fileSwarm = storage::getHostedFileMetaSwarm(fileId);
        if (fileSwarm == nullptr) {
            auto node = storage::getPossibleFileMetaHost(fileId, index);
            transport::ResponseBuffer resp(1 + node->getSerializedSize());
            resp.writeUint8(0);
            resp.writeNode(node);
            co_await transport->sendResponse(resp);
        } else {
            auto *fileMeta = fileSwarm->getFileMeta();
            transport::ResponseBuffer resp(1 + fileMeta->getSerializedSize());
            resp.writeUint8(1);
            resp.writeFileMeta(fileMeta);
            co_await transport->sendResponse(resp);
        }
    }

}