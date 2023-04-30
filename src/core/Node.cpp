#include "Node.h"

namespace RingSwarm::core {
    int Node::getSerializedSize() {
        return 0;
    }

    bool Node::operator==(Node &rhs) const {
        return *id == *rhs.id;
    }

    bool Node::operator!=(Node &rhs) const {
        return !(*this == rhs);
    }

    Node *Node::thisNode = nullptr;
}