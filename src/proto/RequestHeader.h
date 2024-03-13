#ifndef RINGSWARM_REQUESTHEADER_H
#define RINGSWARM_REQUESTHEADER_H

#include <cstdint>

namespace RingSwarm::proto {
struct RequestHeader {
    uint32_t requestLength;
    uint8_t method;
    uint8_t tag;
} __attribute__((packed));
}// namespace RingSwarm::proto

#endif//RINGSWARM_REQUESTHEADER_H
