#ifndef RINGSWARM_NODE_H
#define RINGSWARM_NODE_H

#include "../crypto/AsymmetricalCrypto.h"
#include "Id.h"
#include <memory>

namespace RingSwarm::core {
struct Node {
    std::shared_ptr<Id> id;
    std::shared_ptr<core::PublicKey> publicKey;

    Node() = default;

    Node(const std::shared_ptr<core::PublicKey> &publicKey) : id(publicKey->getId()), publicKey(publicKey) {}

    int getSerializedSize();

    bool operator==(Node &rhs) const;

    bool operator!=(Node &rhs) const;
};
}// namespace RingSwarm::core

#endif//RINGSWARM_NODE_H
