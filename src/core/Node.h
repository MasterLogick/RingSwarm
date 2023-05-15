#ifndef RINGSWARM_NODE_H
#define RINGSWARM_NODE_H

#include "Id.h"
#include "../transport/connectionInfo/ConnectionInfo.h"
#include <memory>
#include "../crypto/AsymmetricalCrypto.h"

namespace RingSwarm::core {
    struct Node {
        Id *id;
        core::PublicKey *publicKey;
        transport::ConnectionInfo *connectionInfo;

        int getSerializedSize();

        bool operator==(Node &rhs) const;

        bool operator!=(Node &rhs) const;

        static Node *thisNode;
    };
}

#endif //RINGSWARM_NODE_H
