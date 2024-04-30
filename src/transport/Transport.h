#ifndef RINGSWARM_TRANSPORT_H
#define RINGSWARM_TRANSPORT_H

#include "../async/coroutine.h"
#include "Buffer.h"
#include <chrono>
#include <memory>

namespace RingSwarm::transport {
class Transport {
public:
    async::Coroutine<transport::Buffer> readBuffer(uint32_t responseLength);

    virtual async::Coroutine<> rawRead(void *data, uint32_t size) = 0;

    virtual void rawWrite(void *data, uint32_t len) = 0;

    virtual void close() = 0;

    virtual ~Transport() = default;
};
}// namespace RingSwarm::transport

#endif//RINGSWARM_TRANSPORT_H
