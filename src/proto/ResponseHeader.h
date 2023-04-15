#ifndef RINGSWARM_RESPONSEHEADER_H
#define RINGSWARM_RESPONSEHEADER_H

#include <cstdint>

namespace RingSwarm::proto {
    struct ResponseHeader {
        uint32_t responseLen;
        bool errorFlag;
    } __attribute__((packed));
}

#endif //RINGSWARM_RESPONSEHEADER_H
