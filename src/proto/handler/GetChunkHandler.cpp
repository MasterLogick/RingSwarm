#include "../ServerHandler.h"
#include "../ClientHandler.h"
#include "../../storage/ChunkStorage.h"

namespace RingSwarm::proto {
    std::shared_ptr<async::Future<uint32_t>>
    ClientHandler::getChunk(core::Id *keyId, uint64_t chunkIndex, uint64_t offset, void *buffer,
                            uint32_t length) {
        RequestBuffer req(32 + 8 + 8 + 4);
        req.write(keyId);
        req.write<uint64_t>(chunkIndex);
        req.write<uint64_t>(offset);
        req.write<uint32_t>(length);
        auto x = transport->rawRead(1)->then<uint32_t>([buffer](uint8_t *data) -> uint32_t {
            memcpy(buffer, data, 1);
            return 1;
        });
        return transport->sendRequest(4, req, length)->then<uint32_t>([&](
                ResponseHeader h) {
            return transport->rawRead(h.responseLen)->then<uint32_t>([buffer, h](uint8_t *data) {
                memcpy(buffer, data, h.responseLen);
                return h.responseLen;
            });
        });
    }

    void ServerHandler::handleGetChunk(transport::Buffer &request, uint8_t tag) {
        auto *keyId = request.read<core::Id *>();
        auto chunkIndex = request.read<uint64_t>();
        auto offset = request.read<uint64_t>();
        auto maxLen = request.read<uint32_t>();
        auto chunk = storage::getMappedChunk(keyId, chunkIndex);
        if (chunk->getSize() <= offset) {
            transport->startLongResponse(0, 1, tag);
            return;
        }
        uint32_t actualSize;
        if (chunk->getSize() - offset > UINT32_MAX) {
            actualSize = maxLen;
        } else {
            actualSize = std::min<uint32_t>(maxLen, chunk->getSize() - offset);
        }
        transport->startLongResponse(actualSize, 1, tag);
        transport->rawWrite(reinterpret_cast<uint8_t *>(chunk->getData()) + offset, actualSize);
    }
}