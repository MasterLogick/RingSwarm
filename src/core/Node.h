#ifndef RINGSWARM_NODE_H
#define RINGSWARM_NODE_H

#include "Id.h"
#include <memory>

namespace RingSwarm::core {
    struct Node {
        uint64_t internalStorageId;
        Id *id;

        int getSerializedSize();

        bool operator==(Node &rhs) const;

        bool operator!=(Node &rhs) const;

        static Node *thisNode;
    };
}

#endif //RINGSWARM_NODE_H
