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

        uint32_t readData(void *buff, uint32_t len, uint64_t offset) override;
    };
}

#endif //RINGSWARM_CACHEDKEYHANDLER_H
