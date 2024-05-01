#ifndef RINGSWARM_ASYMMETRICALCRYPTO_H
#define RINGSWARM_ASYMMETRICALCRYPTO_H

#include "../core/Id.h"
#include "../core/PublicKey.h"

#include "KeyPair.h"

#include <openssl/types.h>

#include <array>
#include <memory>
#include <vector>

extern "C" void EVP_PKEY_CTX_free(EVP_PKEY_CTX *ctx);

namespace RingSwarm::crypto {
// void loadNodeKeys();

// KeyPair genKeyPair();

// std::unique_ptr<EVP_PKEY_CTX, decltype(&EVP_PKEY_CTX_free)>
// initDeriveKeyContext();

}// namespace RingSwarm::crypto

#endif// RINGSWARM_ASYMMETRICALCRYPTO_H
