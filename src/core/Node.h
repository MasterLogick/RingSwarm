#ifndef RINGSWARM_NODE_H
#define RINGSWARM_NODE_H

#include "../crypto/AsymmetricalCrypto.h"
#include "../transport/connectionInfo/ConnectionInfo.h"
#include "Id.h"
#include <memory>

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
}// namespace RingSwarm::core

#endif//RINGSWARM_NODE_H
