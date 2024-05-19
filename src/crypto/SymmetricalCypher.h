#ifndef RINGSWARM_SYMMETRICALCRYPHER_H
#define RINGSWARM_SYMMETRICALCRYPHER_H

#include "../core/PublicKey.h"
#include "../transport/Transport.h"

#include <openssl/evp.h>
#include <openssl/types.h>

#include <vector>

namespace RingSwarm::crypto {
class SymmetricCypher {
    std::unique_ptr<EVP_CIPHER_CTX, decltype(&EVP_CIPHER_CTX_free)> cipher;
    std::unique_ptr<EVP_CIPHER_CTX, decltype(&EVP_CIPHER_CTX_free)> decipher;

public:
    SymmetricCypher(
        std::unique_ptr<EVP_PKEY_CTX, decltype(&EVP_PKEY_CTX_free)> ctx,
        core::PublicKey &serializedPublicKey,
        const uint8_t *iv
    );

    void encode(void *output, void *input, uint32_t size);

    void decode(void *output, void *input, uint32_t size);
};
}// namespace RingSwarm::crypto

#endif// RINGSWARM_SYMMETRICALCRYPHER_H
