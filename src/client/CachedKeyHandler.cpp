#include "CachedKeyHandler.h"
#include "../storage/ChunkStorage.h"

namespace RingSwarm::client {
    std::shared_ptr<async::Future<void *, uint32_t>> CachedKeyHandler::readData(uint32_t len, uint64_t offset) {
        if (offset >= UINT32_MAX - sizeof(core::KeyInfo)) {
            return async::Future<void *, uint32_t>::createResolved(nullptr, 0);
        }
        offset += sizeof(core::KeyInfo);
        if (zeroChunk->getSize() <= offset) {
            return async::Future<void *, uint32_t>::createResolved(nullptr, 0);
        } else if (zeroChunk->getSize() - offset >= UINT32_MAX) {
            return async::Future<void *, uint32_t>::createResolved(
                    reinterpret_cast<uint8_t *>(zeroChunk->getData()) + offset, len);
        } else {
            uint32_t actualSize = std::min<uint32_t>(zeroChunk->getSize() - offset, len);
            return async::Future<void *, uint32_t>::createResolved(
                    reinterpret_cast<uint8_t *>(zeroChunk->getData()) + offset, actualSize);
        }
    }

    CachedKeyHandler::CachedKeyHandler(core::Id *keyId) {
        zeroChunk = storage::getMappedChunk(keyId, 0);
        memcpy(&keyInfo, zeroChunk->getData(), sizeof(core::KeyInfo));
    }

    CachedKeyHandler::~CachedKeyHandler() {

    }

    uint64_t CachedKeyHandler::getDataSize() {
        return zeroChunk->getSize() - sizeof(core::KeyInfo);
    }
}