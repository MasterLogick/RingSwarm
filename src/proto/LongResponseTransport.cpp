#include "LongResponseTransport.h"

#include "../transport/TransportBackendException.h"
#include <utility>

namespace RingSwarm::proto {
std::shared_ptr<async::Future<void>> LongResponseTransport::rawRead(void *data, uint32_t size) {
    throw transport::TransportBackendException();
}

void LongResponseTransport::rawWrite(void *data, uint32_t len) {
    transport::TransportWrapper::rawWrite(data, len);
    responseSize -= len;
    if (len == 0) {
        future->resolve();
    }
}

void LongResponseTransport::close() {
    transport::TransportWrapper::close();
}

LongResponseTransport::LongResponseTransport(
        transport::Transport *transport,
        std::shared_ptr<async::Future<void>> future,
        uint32_t responseSize) : transport::TransportWrapper(transport), future(std::move(future)), responseSize(responseSize) {}

LongResponseTransport::~LongResponseTransport() {
    transport::TransportWrapper::transport = nullptr;
}
}// namespace RingSwarm::proto