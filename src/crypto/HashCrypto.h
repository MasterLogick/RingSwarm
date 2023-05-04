#ifndef RINGSWARM_HASHCRYPTO_H
#define RINGSWARM_HASHCRYPTO_H

#include "../core/Id.h"

namespace RingSwarm::crypto {
    core::Id *hashData(void *data, size_t size);
}

#endif //RINGSWARM_HASHCRYPTO_H
