#ifndef RINGSWARM_NODE_H
#define RINGSWARM_NODE_H

#include "Id.h"

namespace RingSwarm::core {
    class Node {
        Id id;
    public:

        int getSerializedSize();

        Id &getId();

        bool operator==(Node &rhs);

        bool operator!=(Node &rhs);
    };
}

#endif //RINGSWARM_NODE_H
