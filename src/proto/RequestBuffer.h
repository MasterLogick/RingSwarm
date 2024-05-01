#ifndef RINGSWARM_REQUESTBUFFER_H
#define RINGSWARM_REQUESTBUFFER_H

#include "../transport/Buffer.h"

#include "RequestHeader.h"

namespace RingSwarm::proto {
class RequestBuffer : public transport::Buffer {
public:
    RequestBuffer(uint32_t len)
        : Buffer(len + sizeof(RequestHeader), sizeof(RequestHeader)) {}
};
}// namespace RingSwarm::proto

#endif// RINGSWARM_REQUESTBUFFER_H
