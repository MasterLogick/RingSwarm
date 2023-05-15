#ifndef RINGSWARM_SYMMETRICALCRYPHER_H
#define RINGSWARM_SYMMETRICALCRYPHER_H

#include <openssl/types.h>
#include <vector>
#include "../transport/Transport.h"

namespace RingSwarm::crypto {
    class SymmetricCypher {
        std::unique_ptr<EVP_CIPHER_CTX, decltype(&EVP_CIPHER_CTX_free)> cipher;
        std::unique_ptr<EVP_CIPHER_CTX, decltype(&EVP_CIPHER_CTX_free)> decipher;

    public:
        SymmetricCypher(std::unique_ptr<EVP_PKEY_CTX, decltype(&EVP_PKEY_CTX_free)> ctx,
                        core::PublicKey *serializedPublicKey, const uint8_t *iv);

        void write(RingSwarm::transport::Transport *transport, void *data, uint32_t len);

        void read(transport::Transport *transport, void *data, uint32_t len);
    };
}

#endif //RINGSWARM_SYMMETRICALCRYPHER_H
