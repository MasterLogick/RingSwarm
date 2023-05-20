#ifndef RINGSWARM_HASHCRYPTO_H
#define RINGSWARM_HASHCRYPTO_H

#include "../core/Id.h"

namespace RingSwarm::crypto {
    core::Id *hashData(void *data, size_t size);

    core::Id *hashData(auto &data) {
        return hashData(data.data(), data.size());
    }

    core::Id *hashData(auto *data) {
        return hashData(data->data(), data->size());
    }
}

#endif //RINGSWARM_HASHCRYPTO_H
