#ifndef RINGSWARM_KEYHANDLER_H
#define RINGSWARM_KEYHANDLER_H

#include "../async/Future.h"

#include <cstdint>

namespace RingSwarm::client {
class KeyHandler {
public:
    virtual std::shared_ptr<async::Future<void *, uint32_t>> readData(uint32_t len, uint64_t offset) = 0;

    virtual uint64_t getDataSize() = 0;

    virtual ~KeyHandler() = default;
};
}// namespace RingSwarm::client

#endif//RINGSWARM_KEYHANDLER_H
