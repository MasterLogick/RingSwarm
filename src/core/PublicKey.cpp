#include "PublicKey.h"
#include "../crypto/CryptoException.h"
#include "../crypto/HashCrypto.h"
#include <openssl/core_names.h>
#include <openssl/evp.h>

namespace RingSwarm::core {

Id *PublicKey::getId() {
    return crypto::hashData(this);
}

bool PublicKey::verifyData(std::vector<char> &data, crypto::Signature *sign) {
    OSSL_PARAM params[] = {
            OSSL_PARAM_utf8_string(OSSL_PKEY_PARAM_GROUP_NAME, (void *) SN_secp256k1, strlen(SN_secp256k1)),
            OSSL_PARAM_octet_string(OSSL_PKEY_PARAM_PUB_KEY, this->data(), size()),
            OSSL_PARAM_END};
    std::unique_ptr<EVP_PKEY_CTX, decltype(&EVP_PKEY_CTX_free)> publicKeyCtx(
            EVP_PKEY_CTX_new_from_name(nullptr, "EC", nullptr),
            EVP_PKEY_CTX_free);
    if (publicKeyCtx == nullptr) {
        throw crypto::CryptoException();
    }
    if (EVP_PKEY_fromdata_init(publicKeyCtx.get()) != 1) {
        throw crypto::CryptoException();
    }

    EVP_PKEY *ptr = nullptr;
    if (EVP_PKEY_fromdata(publicKeyCtx.get(), &ptr,
                          EVP_PKEY_PUBLIC_KEY, params) != 1) {
        throw crypto::CryptoException();
    }
    std::unique_ptr<EVP_PKEY, decltype(&EVP_PKEY_free)> publicKey(ptr, EVP_PKEY_free);

    std::unique_ptr<EVP_MD_CTX, decltype(&EVP_MD_CTX_free)> mdCtx(EVP_MD_CTX_new(), EVP_MD_CTX_free);
    if (EVP_DigestVerifyInit(mdCtx.get(), nullptr, EVP_sha3_256(), nullptr, publicKey.get()) != 1) {
        throw crypto::CryptoException();
    }
    if (EVP_DigestVerifyUpdate(mdCtx.get(), data.data(), data.size()) != 1) {
        throw crypto::CryptoException();
    }

    return EVP_DigestVerifyFinal(mdCtx.get(), cbegin(), size()) == 1;
}
}// namespace RingSwarm::core