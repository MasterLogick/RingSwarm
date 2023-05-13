#ifndef RINGSWARM_ASYMMETRICALCRYPTO_H
#define RINGSWARM_ASYMMETRICALCRYPTO_H

#include <vector>
#include <openssl/types.h>
#include "../core/Id.h"

namespace RingSwarm::crypto {
    std::vector<char> signData(void *data, size_t size);

    bool verifyData(std::vector<char> &data, std::vector<char> &sig, std::vector<char> &pubKey);

    void loadNodeKeys();

    EVP_PKEY_CTX *initDeriveKeyContext();

#define SERIALIZED_NODE_PUBLIC_KEY_LENGTH 44
#define RAW_NODE_PUBLIC_KEY_LENGTH 33
}

#endif //RINGSWARM_ASYMMETRICALCRYPTO_H
