#ifndef RINGSWARM_CONNECTIONINFO_H
#define RINGSWARM_CONNECTIONINFO_H

#include <vector>
#include <cstdint>
#include "../Buffer.h"

namespace RingSwarm::core {
    class Node;

}
namespace RingSwarm::transport {
    class Transport;

    class ConnectionInfo {
    public:
        virtual Transport *openConnection() = 0;

        virtual uint32_t getSerializedSize() = 0;

        virtual void serialize(Buffer &buffer) = 0;

        static ConnectionInfo *parseConnectionInfo(Buffer &buffer);
    };
}

#endif //RINGSWARM_CONNECTIONINFO_H
