#include "HashCrypto.h"
#include "CryptoException.h"
#include <openssl/evp.h>

namespace RingSwarm::crypto {
    core::Id *hashData(void *data, size_t size) {
        std::unique_ptr<EVP_MD_CTX, decltype(&EVP_MD_CTX_free)> ctx(EVP_MD_CTX_new(), EVP_MD_CTX_free);
        if (ctx == nullptr) {
            throw CryptoException();
        }
        if (EVP_DigestInit_ex2(ctx.get(), EVP_sha3_256(), nullptr) != 1) {
            throw CryptoException();
        }
        if (EVP_DigestUpdate(ctx.get(), data, size) != 1) {
            throw CryptoException();
        }
        auto *id = new core::Id();
        uint out;
        if (EVP_DigestFinal_ex(ctx.get(), id->hash, &out) != 1) {
            delete id;
            throw CryptoException();
        }
        return id;
    }
}