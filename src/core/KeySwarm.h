#ifndef RINGSWARM_KEYSWARM_H
#define RINGSWARM_KEYSWARM_H

#include <map>
#include <vector>
#include "Node.h"
#include "ChunkRing.h"

namespace RingSwarm::core {
    struct KeySwarm {
        core::Id *keyId;
        core::PublicKey *key;
        std::map<uint8_t, Node *> swarm;
        ChunkRing *ring;
    };
}

#endif //RINGSWARM_KEYSWARM_H
