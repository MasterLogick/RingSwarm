#ifndef RINGSWARM_KEYHANDLER_H
#define RINGSWARM_KEYHANDLER_H

#include <cstdint>

namespace RingSwarm::client {
    class KeyHandler {
    public:
        virtual uint32_t readData(void *buff, uint32_t len, uint64_t offset) = 0;
    };
}

#endif //RINGSWARM_KEYHANDLER_H
