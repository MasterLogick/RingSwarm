#ifndef RINGSWARM_RESPONSEHEADER_H
#define RINGSWARM_RESPONSEHEADER_H

#include <cstdint>

namespace RingSwarm::proto {
struct ResponseHeader {
    uint32_t responseLen;
    uint16_t tag;
} __attribute__((packed));
}// namespace RingSwarm::proto

#endif// RINGSWARM_RESPONSEHEADER_H
