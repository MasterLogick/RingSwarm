#ifndef RINGSWARM_NODE_H
#define RINGSWARM_NODE_H

#include "Id.h"
#include "ConnectionInfo.h"
#include <memory>

namespace RingSwarm::core {
    struct Node {
        Id *id;
        std::vector<char> publicKey;
        ConnectionInfo connectionInfo;

        int getSerializedSize();

        bool operator==(Node &rhs) const;

        bool operator!=(Node &rhs) const;

        static Node *thisNode;
    };
}

#endif //RINGSWARM_NODE_H
