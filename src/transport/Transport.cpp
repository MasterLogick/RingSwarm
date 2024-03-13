#include "Transport.h"

namespace RingSwarm::transport {
std::shared_ptr<async::Future<std::shared_ptr<transport::Buffer>>> Transport::readBuffer(uint32_t responseLength) {
    auto *data = new uint8_t[responseLength];
    return rawRead(data, responseLength)->then<std::shared_ptr<transport::Buffer>>([data, responseLength]() {
        return std::make_shared<transport::Buffer>(data, responseLength);
    });
}
}// namespace RingSwarm::transport