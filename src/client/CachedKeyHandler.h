#ifndef RINGSWARM_CACHEDKEYHANDLER_H
#define RINGSWARM_CACHEDKEYHANDLER_H

#include <memory>
#include "../core/Id.h"
#include "KeyHandler.h"
#include "../core/KeyInfo.h"
#include "../storage/MappedChunk.h"

namespace RingSwarm::client {
    class CachedKeyHandler : public KeyHandler {
        core::KeyInfo keyInfo;
        std::shared_ptr<storage::MappedChunk> zeroChunk;

    public:
        explicit CachedKeyHandler(core::Id *keyId);

        std::shared_ptr<async::Future<void *, uint32_t>> readData(uint32_t len, uint64_t offset) override;

        uint64_t getDataSize() override;

        ~CachedKeyHandler() override;
    };
}

#endif //RINGSWARM_CACHEDKEYHANDLER_H
