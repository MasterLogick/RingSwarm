#ifndef RINGSWARM_TRANSPORT_H
#define RINGSWARM_TRANSPORT_H

#include "../async/Future.h"
#include "Buffer.h"
#include <chrono>
#include <memory>

namespace RingSwarm::transport {
class Transport {
public:
    std::shared_ptr<async::Future<std::shared_ptr<transport::Buffer>>> readBuffer(uint32_t responseLength);

    virtual std::shared_ptr<async::Future<void>> rawRead(void *data, uint32_t size) = 0;

    virtual void rawWrite(void *data, uint32_t len) = 0;

    virtual void close() = 0;

    virtual ~Transport() = default;
};
}// namespace RingSwarm::transport

#endif//RINGSWARM_TRANSPORT_H
