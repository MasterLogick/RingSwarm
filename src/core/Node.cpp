#include "Node.h"
#include "../crypto/AsymmetricalCrypto.h"

namespace RingSwarm::core {
    int Node::getSerializedSize() {
        return publicKey->size() + connectionInfo->getSerializedSize();
    }

    bool Node::operator==(Node &rhs) const {
        return *id == *rhs.id;
    }

    bool Node::operator!=(Node &rhs) const {
        return !(*this == rhs);
    }

    Node *Node::thisNode = new Node();
}