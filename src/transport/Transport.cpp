#include "Transport.h"

namespace RingSwarm::transport {
async::Coroutine<transport::Buffer>
Transport::readBuffer(uint32_t responseLength) {
    auto *data = new uint8_t[responseLength];
    ~co_await rawRead(data, responseLength);
    co_return transport::Buffer(data, responseLength);
}
}// namespace RingSwarm::transport
