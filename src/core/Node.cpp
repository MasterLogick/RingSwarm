#include "Node.h"

#include "../crypto/AsymmetricalCrypto.h"

namespace RingSwarm::core {
int Node::getSerializedSize() {
    return publicKey->size();
}

bool Node::operator==(Node &rhs) const {
    return *id == *rhs.id;
}

bool Node::operator!=(Node &rhs) const {
    return !(*this == rhs);
}
}// namespace RingSwarm::core
