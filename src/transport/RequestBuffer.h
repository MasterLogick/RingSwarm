#ifndef RINGSWARM_REQUESTBUFFER_H
#define RINGSWARM_REQUESTBUFFER_H

#include "Buffer.h"

namespace RingSwarm::transport {
    class RequestBuffer : public Buffer {
    public:
        RequestBuffer(uint32_t len) : Buffer(len + 6, 6) {}
    };
}

#endif //RINGSWARM_REQUESTBUFFER_H
