#include "../ServerHandler.h"
#include "../ClientHandler.h"
#include "../../storage/ChunkStorage.h"

namespace RingSwarm::proto {
    uint32_t ClientHandler::getChunk(
            core::Id *keyId,
            uint64_t chunkIndex,
            uint64_t offset,
            void *buffer,
            uint32_t length
    ) {
        transport::RequestBuffer req(32 + 8 + 8 + 4);
        req.write(keyId);
        req.write<uint64_t>(chunkIndex);
        req.write<uint64_t>(offset);
        req.write<uint32_t>(length);
        transport->sendRequest(4, req);
        uint32_t len = transport->readResponseLength(length);
        transport->rawRead(buffer, len);
        return len;
    }

    void ServerHandler::handleGetChunk(transport::Buffer &request) {
        auto *keyId = request.read<core::Id *>();
        auto chunkIndex = request.read<uint64_t>();
        auto offset = request.read<uint64_t>();
        auto maxLen = request.read<uint32_t>();
        auto chunk = storage::getMappedChunk(keyId, chunkIndex);
        if (chunk->getSize() <= offset) {
            transport->startLongResponse(0);
            return;
        }
        uint32_t actualSize;
        if (chunk->getSize() - offset > UINT32_MAX) {
            actualSize = maxLen;
        } else {
            actualSize = std::min<uint32_t>(maxLen, chunk->getSize() - offset);
        }
        transport->startLongResponse(actualSize);
        transport->rawWrite(reinterpret_cast<uint8_t *>(chunk->getData()) + offset, actualSize);
    }
}