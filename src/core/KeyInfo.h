#ifndef RINGSWARM_KEYINFO_H
#define RINGSWARM_KEYINFO_H

#include "Id.h"

#define PACKED __attribute__((packed))

namespace RingSwarm::core {
struct KeyInfo {
    uint64_t chunksCount;
    uint64_t maxChunkSize;
    uint8_t minSwarmSize;
    uint8_t minRingConnectivity;

    enum Type {
        ONE_FILE = 1,
        DIRECTORY = 2,
        // STREAM = 3
    } PACKED type;
} PACKED;
}// namespace RingSwarm::core

#endif// RINGSWARM_KEYINFO_H
