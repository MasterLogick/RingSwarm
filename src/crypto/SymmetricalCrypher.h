#ifndef RINGSWARM_SYMMETRICALCRYPHER_H
#define RINGSWARM_SYMMETRICALCRYPHER_H

#include "../transport/Transport.h"
#include <openssl/types.h>
#include <openssl/evp.h>
#include <vector>

namespace RingSwarm::crypto {
class SymmetricCypher {
    std::unique_ptr<EVP_CIPHER_CTX, decltype(&EVP_CIPHER_CTX_free)> cipher;
    std::unique_ptr<EVP_CIPHER_CTX, decltype(&EVP_CIPHER_CTX_free)> decipher;

public:
    SymmetricCypher(std::unique_ptr<EVP_PKEY_CTX, decltype(&EVP_PKEY_CTX_free)> ctx,
                    core::PublicKey *serializedPublicKey, const uint8_t *iv);

    void encode(void *data, uint32_t size);
    //        void write(RingSwarm::transport::Transport *transport, void *data, uint32_t len);

    //        uint32_t rawNonBlockingRead(transport::Transport *transport, void *data, uint32_t len);

    void decode(uint8_t *data, uint32_t size);
};
}// namespace RingSwarm::crypto

#endif//RINGSWARM_SYMMETRICALCRYPHER_H
