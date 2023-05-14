#ifndef RINGSWARM_RESPONSEBUFFER_H
#define RINGSWARM_RESPONSEBUFFER_H

#include "Buffer.h"
#include "../core/ChunkSwarm.h"

namespace RingSwarm::transport {
    class ResponseBuffer : public Buffer {
    public:
        ResponseBuffer(uint32_t len) : Buffer(len + 5, 5) {}
    };
}

#endif //RINGSWARM_RESPONSEBUFFER_H
