#include "../ServerHandler.h"
#include "../ClientHandler.h"

#define MAX_RESPONSE_SIZE (0)
namespace RingSwarm::proto {
    void ClientHandler::subscribeOnChunkChange(core::Id *fileId, uint64_t chunkIndex) {
        transport::RequestBuffer req(40);
        req.writeId(fileId);
        req.writeUint64(chunkIndex);
        transport->sendRequest(11, req);
        transport->readResponse(MAX_RESPONSE_SIZE);
    }

    void ServerHandler::handleSubscribeOnChunkChange(transport::Buffer &request) {
        transport->sendEmptyResponse();
    }
}