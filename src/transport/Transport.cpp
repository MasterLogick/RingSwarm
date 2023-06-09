#include "Transport.h"

namespace RingSwarm::transport {
    std::shared_ptr<async::Future<transport::Buffer>>
    Transport::readBuffer(uint32_t responseLength) {
        return rawRead(responseLength)->then<transport::Buffer>(
                [responseLength](uint8_t *data) -> transport::Buffer {
                    return transport::Buffer(data, responseLength);
                });
    }
}