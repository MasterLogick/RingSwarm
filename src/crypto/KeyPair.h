#ifndef RINGSWARM_KEYPAIR_H
#define RINGSWARM_KEYPAIR_H

#include <memory>
#include <openssl/types.h>

namespace RingSwarm::core {
    class PublicKey;
}
namespace RingSwarm::crypto {
    class Signature;

    struct KeyPair {
        core::PublicKey *const publicKey;
        EVP_PKEY *privateKey;
    public:
        KeyPair();

        Signature *signData(void *data, size_t size);

        ~KeyPair();
    };
}

#endif //RINGSWARM_KEYPAIR_H
