#ifndef RINGSWARM_ASYMMETRICALCRYPTO_H
#define RINGSWARM_ASYMMETRICALCRYPTO_H

#include <vector>
#include <openssl/types.h>
#include "../core/Id.h"
#include <array>
#include <memory>

extern "C"
void EVP_PKEY_CTX_free(EVP_PKEY_CTX *ctx);

namespace RingSwarm::crypto {

    using PublicKey = std::array<uint8_t, 33>;

    using Signature = std::array<uint8_t, 72>;

    Signature *signData(void *data, size_t size);

    bool verifyData(std::vector<char> &data, Signature *sig, PublicKey *pubKey);

    void loadNodeKeys();

    std::unique_ptr<EVP_PKEY_CTX, decltype(&EVP_PKEY_CTX_free)> initDeriveKeyContext();

}

#endif //RINGSWARM_ASYMMETRICALCRYPTO_H
