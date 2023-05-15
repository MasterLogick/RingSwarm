#include "../ServerHandler.h"
#include "../ClientHandler.h"

#define MAX_RESPONSE_SIZE (0)
namespace RingSwarm::proto {
    void ClientHandler::unsubscribeOnChunkChange(core::Id *fileId, uint64_t chunkIndex) {
        transport::RequestBuffer req(40);
        req.write(fileId);
        req.write<uint64_t>(chunkIndex);
        transport->sendRequest(12, req);
        transport->readResponse(MAX_RESPONSE_SIZE);
    }

    void ServerHandler::handleUnsubscribeOnChunkChange(transport::Buffer &request) {
        transport->sendEmptyResponse();
    }
}