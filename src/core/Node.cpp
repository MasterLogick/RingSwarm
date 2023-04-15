#include "Node.h"

namespace RingSwarm::core {
    int Node::getSerializedSize() {
        return 0;
    }

    bool Node::operator==(Node &rhs) {
        return getId() == rhs.getId();
    }

    bool Node::operator!=(Node &rhs) {
        return !(*this == rhs);
    }

    Id &Node::getId() {
        return id;
    }
}