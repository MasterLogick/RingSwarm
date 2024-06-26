#ifndef RINGSWARM_RESPONSEBUFFER_H
#define RINGSWARM_RESPONSEBUFFER_H

#include "../transport/Buffer.h"

#include "ResponseHeader.h"

namespace RingSwarm::proto {
class ResponseBuffer : public transport::Buffer {
public:
    ResponseBuffer(uint32_t len)
        : Buffer(len + sizeof(ResponseHeader), sizeof(ResponseHeader)) {}
};
}// namespace RingSwarm::proto

#endif// RINGSWARM_RESPONSEBUFFER_H
