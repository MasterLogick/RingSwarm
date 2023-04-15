#ifndef RINGSWARM_REQUESTHEADER_H
#define RINGSWARM_REQUESTHEADER_H

#include <cstdint>

namespace RingSwarm::proto {
    struct RequestHeader {
        uint32_t requestLength;
        uint16_t method;
    } __attribute__((packed));
}

#endif //RINGSWARM_REQUESTHEADER_H
