#ifndef RINGSWARM_SYMMETRICALCRYPHER_H
#define RINGSWARM_SYMMETRICALCRYPHER_H

#include <openssl/types.h>
#include <vector>
#include "../transport/Transport.h"

namespace RingSwarm::crypto {
    class SymmetricCypher {
        EVP_CIPHER_CTX *cipher, *decipher;

        void initCypher(EVP_PKEY_CTX *ctx, std::vector<char> &serializedPubKey, uint8_t *iv);

    public:
        SymmetricCypher(EVP_PKEY *privateKey, std::vector<char> &remotePublicKey, uint8_t *iv);

        SymmetricCypher(std::vector<char> &remotePublicKey, uint8_t *iv);

        void write(RingSwarm::transport::Transport *transport, void *data, uint32_t len);

        void read(transport::Transport *transport, void *data, uint32_t len);

        ~SymmetricCypher();
    };
}

#endif //RINGSWARM_SYMMETRICALCRYPHER_H
