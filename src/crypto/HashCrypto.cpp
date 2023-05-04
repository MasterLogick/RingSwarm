#include "HashCrypto.h"
#include <openssl/sha.h>

namespace RingSwarm::crypto {
    core::Id *hashData(const unsigned char *data, size_t size) {
        auto *id = new core::Id();
        if (SHA256(data, size, id->hash) != nullptr) {
            return id;
        } else {
            delete id;
            return nullptr;
        }
    }
}