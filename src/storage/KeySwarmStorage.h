#ifndef RINGSWARM_KEYSWARMSTORAGE_H
#define RINGSWARM_KEYSWARMSTORAGE_H

#include "../core/Id.h"
#include "../core/KeySwarm.h"

namespace RingSwarm::storage {
    core::KeySwarm *getKeySwarm(core::Id *keyId);

    std::vector<core::Id *> getAllStoredKeySwarms();

    void storeKeySwarm(core::KeySwarm *keySwarm);
}
#endif //RINGSWARM_KEYSWARMSTORAGE_H
