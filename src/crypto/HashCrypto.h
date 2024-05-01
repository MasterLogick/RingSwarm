#ifndef RINGSWARM_HASHCRYPTO_H
#define RINGSWARM_HASHCRYPTO_H

#include "../core/Id.h"

#include <memory>

namespace RingSwarm::crypto {
std::shared_ptr<core::Id> hashData(void *data, size_t size);

std::shared_ptr<core::Id> hashData(auto &data) {
    return hashData(data.data(), data.size());
}

std::shared_ptr<core::Id> hashData(auto *data) {
    return hashData(data->data(), data->size());
}
}// namespace RingSwarm::crypto

#endif// RINGSWARM_HASHCRYPTO_H
