#ifndef RINGSWARM_REQUESTHEADER_H
#define RINGSWARM_REQUESTHEADER_H

#include "CommandId.h"

#include <cstdint>

namespace RingSwarm::proto {
struct RequestHeader {
    uint32_t requestLength;
    CommandId method;
    uint16_t tag;
} __attribute__((packed));
}// namespace RingSwarm::proto

#endif// RINGSWARM_REQUESTHEADER_H
