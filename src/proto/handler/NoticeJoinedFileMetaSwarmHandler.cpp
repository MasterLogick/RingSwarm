#include "../ServerHandler.h"
#include "../../storage/FileSwarmStorage.h"
#include "../ClientHandler.h"

#define MAX_RESPONSE_SIZE (256 * 8 * 1024)
namespace RingSwarm::proto {
    std::vector<core::Node *> ClientHandler::noticeJoinedFileMetaSwarm(core::Id *fileId, uint8_t index) {
        transport::RequestBuffer req(33);
        req.writeId(fileId);
        req.writeUint8(index);
        transport->sendRequest(6, req);
        auto resp = transport->readResponse(MAX_RESPONSE_SIZE);
        return resp.readNodeList();
    }

    void ServerHandler::handleNoticeJoinedFileMetaSwarm(transport::Buffer &request) {
        auto id = request.readId();
        auto index = request.readUint8();
        auto fileMetaSwarm = storage::getFileMetaSwarm(id);
        if (fileMetaSwarm == nullptr) {
            transport->sendError();
        } else {
            auto &swarm = fileMetaSwarm->swarm;
            if (std::none_of(swarm.begin(), swarm.end(),
                             [&](auto pair) -> bool { return *pair.second == *remote; })) {
                swarm[index] = remote;
            }
            //todo fix
//            sendNodeListResponse(swarm);
        }
    }
}