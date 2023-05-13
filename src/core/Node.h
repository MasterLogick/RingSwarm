#ifndef RINGSWARM_NODE_H
#define RINGSWARM_NODE_H

#include "Id.h"
#include "../transport/connectionInfo/ConnectionInfo.h"
#include <memory>

namespace RingSwarm::core {
    struct Node {
        Id *id;
        std::vector<char> publicKey;
        transport::ConnectionInfo *connectionInfo;

        int getSerializedSize();

        void deriveId();

        bool operator==(Node &rhs) const;

        bool operator!=(Node &rhs) const;

        static Node *thisNode;
    };
}

#endif //RINGSWARM_NODE_H
