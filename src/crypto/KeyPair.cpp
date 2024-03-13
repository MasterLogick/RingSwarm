#include "KeyPair.h"
#include "../core/PublicKey.h"
#include "CryptoException.h"
#include <openssl/core_names.h>
#include <openssl/ec.h>
#include <openssl/evp.h>

namespace RingSwarm::crypto {
KeyPair::KeyPair() : publicKey(new core::PublicKey()), privateKey(EVP_EC_gen(SN_secp256k1)) {
    size_t rawPubKeyLen = publicKey->size();
    if (EVP_PKEY_get_octet_string_param(privateKey,
                                        OSSL_PKEY_PARAM_PUB_KEY,
                                        publicKey->data(),
                                        rawPubKeyLen,
                                        &rawPubKeyLen) != 1) {
        throw CryptoException();
    }
}

KeyPair::~KeyPair() {
    delete publicKey;
    EVP_PKEY_free(privateKey);
}

Signature *KeyPair::signData(void *data, size_t size) {
    std::unique_ptr<EVP_MD_CTX, decltype(&EVP_MD_CTX_free)> mdCtx(EVP_MD_CTX_create(), EVP_MD_CTX_free);
    if (mdCtx == nullptr) {
        throw CryptoException();
    }
    if (EVP_DigestSignInit(mdCtx.get(), nullptr, EVP_sha3_256(), nullptr, privateKey) != 1) {
        throw CryptoException();
    }
    if (EVP_DigestSignUpdate(mdCtx.get(), data, size) != 1) {
        throw CryptoException();
    }
    auto sign = new Signature();
    size_t len = sign->size();
    if (EVP_DigestSignFinal(mdCtx.get(), sign->data(), &len) != 1) {
        throw CryptoException();
    }
    return sign;
}
}// namespace RingSwarm::crypto