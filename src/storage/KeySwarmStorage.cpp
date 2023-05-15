#include "KeySwarmStorage.h"
#include "ChunkRingStorage.h"
#include "ClonedEntityException.h"

namespace RingSwarm::storage {
    std::map<core::Id *, core::KeySwarm *, core::Id::Comparator> keySwarmStorage;

    core::KeySwarm *getKeySwarm(core::Id *keyId) {
        if (keySwarmStorage.contains(keyId)) {
            return keySwarmStorage[keyId];
        }
        return nullptr;
    }

    void storeKeySwarm(core::KeySwarm *keySwarm) {
        if (keySwarmStorage.contains(keySwarm->keyId)) {
            if (keySwarmStorage[keySwarm->keyId] != keySwarm) {
                throw ClonedEntityException();
            }
            return;
        } else {
            keySwarmStorage[keySwarm->keyId] = keySwarm;
        }
        storeChunkRing(keySwarm->keyId, keySwarm->ring);
    }
}