#ifndef RINGSWARM_KEYSWARM_H
#define RINGSWARM_KEYSWARM_H

#include "ChunkRing.h"
#include "Node.h"
#include <map>
#include <vector>

namespace RingSwarm::core {
struct KeySwarm {
    core::Id *keyId;
    core::PublicKey *key;
    std::map<uint8_t, Node *> swarm;
    ChunkRing *ring;
};
}// namespace RingSwarm::core

#endif//RINGSWARM_KEYSWARM_H
