#include "../../storage/ChunkStorage.h"
#include "../ClientHandler.h"
#include "../ServerHandler.h"

namespace RingSwarm::proto {
std::shared_ptr<async::Future<uint32_t>> ClientHandler::getChunk(
    core::Id *keyId,
    uint64_t chunkIndex,
    uint64_t offset,
    void *buffer,
    uint32_t length
) {
    RequestBuffer req(32 + 8 + 8 + 4);
    req.write(keyId);
    req.write<uint64_t>(chunkIndex);
    req.write<uint64_t>(offset);
    req.write<uint32_t>(length);
    return transport->sendRequest(4, req, length)
        ->then<uint32_t>([this, buffer](ResponseHeader h) {
            if (h.responseLen == 0) {
                return async::Future<uint32_t>::createResolved(0);
            }
            return transport->rawRead(buffer, h.responseLen)
                ->then<uint32_t>([rl = h.responseLen]() { return rl; });
        });
}

void ServerHandler::handleGetChunk(transport::Buffer &request, uint8_t tag) {
    auto *keyId = request.read<core::Id *>();
    auto chunkIndex = request.read<uint64_t>();
    auto offset = request.read<uint64_t>();
    auto maxLen = request.read<uint32_t>();
    auto chunk = storage::getMappedChunk(keyId, chunkIndex);
    if (chunk->getSize() <= offset) {
        transport->sendEmptyResponse(1, tag);
        return;
    }
    uint32_t actualSize;
    if (chunk->getSize() - offset > UINT32_MAX) {
        actualSize = maxLen;
    } else {
        actualSize = std::min<uint32_t>(maxLen, chunk->getSize() - offset);
    }
    transport->scheduleLongResponse(actualSize, 1, tag)
        ->then([actualSize, chunk, offset](auto lrt) {
            uint32_t buffSize = 1024 * 1024;
            char buf[buffSize];
            for (uint32_t i = 0, sz = buffSize; sz == buffSize; i += buffSize) {
                sz = std::min<uint32_t>(actualSize - i, buffSize);
                memcpy(
                    buf,
                    reinterpret_cast<uint8_t *>(chunk->getData()) + offset + i,
                    sz
                );
                lrt->rawWrite(buf, sz);
            }
        });
}
}// namespace RingSwarm::proto
