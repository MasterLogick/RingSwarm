#include "AsymmetricalCrypto.h"
#include "CryptoException.h"
#include "HashCrypto.h"
#include "../core/Settings.h"
#include "../core/Node.h"
#include "../storage/NodeStorage.h"
#include <openssl/param_build.h>
#include <openssl/core_names.h>
#include <openssl/objects.h>
#include <openssl/evp.h>
#include <openssl/ec.h>
#include <boost/log/trivial.hpp>
#include <boost/algorithm/hex.hpp>

namespace RingSwarm::crypto {
    PublicKey *nodePubKey;
    EVP_PKEY *nodePrivKey;

    Signature *signData(void *data, size_t size) {
        std::unique_ptr<EVP_MD_CTX, decltype(&EVP_MD_CTX_free)> mdCtx(EVP_MD_CTX_create(), EVP_MD_CTX_free);
        if (mdCtx == nullptr) {
            throw CryptoException();
        }
        if (EVP_DigestSignInit(mdCtx.get(), nullptr, EVP_sha256(), nullptr, nodePrivKey) != 1) {
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

    bool verifyData(std::vector<char> &data, Signature &sig, PublicKey &pubKey) {
        OSSL_PARAM params[] = {
                OSSL_PARAM_utf8_string(OSSL_PKEY_PARAM_GROUP_NAME, (void *) SN_secp256k1, strlen(SN_secp256k1)),
                OSSL_PARAM_octet_string(OSSL_PKEY_PARAM_PUB_KEY, pubKey.data(), pubKey.size()),
                OSSL_PARAM_END};
        std::unique_ptr<EVP_PKEY_CTX, decltype(&EVP_PKEY_CTX_free)> publicKeyCtx(
                EVP_PKEY_CTX_new_from_name(nullptr, "EC", nullptr),
                EVP_PKEY_CTX_free);
        if (publicKeyCtx == nullptr) {
            throw CryptoException();
        }
        if (EVP_PKEY_fromdata_init(publicKeyCtx.get()) != 1) {
            throw CryptoException();
        }

        EVP_PKEY *ptr = nullptr;
        if (EVP_PKEY_fromdata(publicKeyCtx.get(), &ptr,
                              EVP_PKEY_PUBLIC_KEY, params) != 1) {
            throw CryptoException();
        }
        std::unique_ptr<EVP_PKEY, decltype(&EVP_PKEY_free)> publicKey(ptr, EVP_PKEY_free);

        std::unique_ptr<EVP_MD_CTX, decltype(&EVP_MD_CTX_free)> mdCtx(EVP_MD_CTX_new(), EVP_MD_CTX_free);
        if (EVP_DigestVerifyInit(mdCtx.get(), nullptr, EVP_sha256(), nullptr, publicKey.get()) != 1) {
            throw CryptoException();
        }
        if (EVP_DigestVerifyUpdate(mdCtx.get(), data.data(), data.size()) != 1) {
            throw CryptoException();
        }

        return EVP_DigestVerifyFinal(mdCtx.get(), pubKey.cbegin(), pubKey.size()) == 1;
    }

    void genNewKeyPair() {
        std::unique_ptr<EVP_PKEY, decltype(&EVP_PKEY_free)> keyPair(EVP_EC_gen(SN_secp256k1), EVP_PKEY_free);
        PublicKey rawPubKey;
        size_t rawPubKeyLen = rawPubKey.size();
        if (EVP_PKEY_get_octet_string_param(keyPair.get(),
                                            OSSL_PKEY_PARAM_PUB_KEY,
                                            reinterpret_cast<unsigned char *>(rawPubKey.data()),
                                            rawPubKeyLen,
                                            &rawPubKeyLen) != 1) {
            throw CryptoException();
        }

        std::string serializedPubKey;
        boost::algorithm::hex(rawPubKey.begin(), rawPubKey.end(), std::back_inserter(serializedPubKey));
        BIGNUM *privateKey = nullptr;
        if (EVP_PKEY_get_bn_param(keyPair.get(), OSSL_PKEY_PARAM_PRIV_KEY, &privateKey) != 1) {
            throw CryptoException();
        }
        char *serializedPrivKey = BN_bn2hex(privateKey);
        core::setSetting("public key", serializedPubKey.c_str());
        core::setSetting("private key", serializedPrivKey);
        OPENSSL_free(serializedPrivKey);
        BN_free(privateKey);
        loadNodeKeys();
    }

    void loadPubKey(std::string &serializedPubKey) {
        nodePubKey = new PublicKey();
        boost::algorithm::unhex(serializedPubKey, nodePubKey->begin());
    }

    void loadPrivKey(std::string &serializedPrivKey) {
        /*
                BIGNUM *ptr = nullptr;
        if (BN_hex2bn(&ptr, serializedPrivKey.c_str()) == 0) {
            throw CryptoException();
        }
        std::unique_ptr<BIGNUM, decltype(&BN_free)> privKeyBn(ptr, BN_free);
        OSSL_PARAM params[] = {
                OSSL_PARAM_utf8_string(OSSL_PKEY_PARAM_GROUP_NAME, (void *) SN_secp256k1, strlen(SN_secp256k1)),
                OSSL_PARAM_BN(OSSL_PKEY_PARAM_PRIV_KEY, privKeyBn.get(), (size_t) BN_num_bytes(privKeyBn.get())),
                OSSL_PARAM_END
        };
        std::unique_ptr<EVP_PKEY_CTX, decltype(&EVP_PKEY_CTX_free)> privateKeyCtx(
                EVP_PKEY_CTX_new_from_name(nullptr, "EC", nullptr),
                EVP_PKEY_CTX_free);
        if (privateKeyCtx == nullptr) {
            throw CryptoException();
        }
        if (EVP_PKEY_fromdata_init(privateKeyCtx.get()) != 1) {
            throw CryptoException();
        }
        if (EVP_PKEY_fromdata(privateKeyCtx.get(), &nodePrivKey, EVP_PKEY_KEYPAIR, params) != 1) {
            throw CryptoException();
        }
         * */
        std::unique_ptr<OSSL_PARAM_BLD, decltype(&OSSL_PARAM_BLD_free)> paramBuild(
                OSSL_PARAM_BLD_new(),
                OSSL_PARAM_BLD_free);
        if (paramBuild == nullptr) {
            throw CryptoException();
        }
        if (OSSL_PARAM_BLD_push_utf8_string(paramBuild.get(),
                                            OSSL_PKEY_PARAM_GROUP_NAME,
                                            SN_secp256k1,
                                            0) != 1) {
            throw CryptoException();
        }
        BIGNUM *ptr = nullptr;
        if (BN_hex2bn(&ptr, serializedPrivKey.c_str()) == 0) {
            throw CryptoException();
        }
        std::unique_ptr<BIGNUM, decltype(&BN_free)> privKeyBn(ptr, &BN_free);
        if (OSSL_PARAM_BLD_push_BN(paramBuild.get(),
                                   OSSL_PKEY_PARAM_PRIV_KEY,
                                   privKeyBn.get()) != 1) {
            throw CryptoException();
        }
        std::unique_ptr<OSSL_PARAM, decltype(&OSSL_PARAM_free)> params(
                OSSL_PARAM_BLD_to_param(paramBuild.get()),
                OSSL_PARAM_free);
        if (params == nullptr) {
            throw CryptoException();
        }
        std::unique_ptr<EVP_PKEY_CTX, decltype(&EVP_PKEY_CTX_free)> privateKeyCtx(
                EVP_PKEY_CTX_new_from_name(nullptr, "EC", nullptr),
                EVP_PKEY_CTX_free);
        if (privateKeyCtx == nullptr) {
            throw CryptoException();
        }
        if (EVP_PKEY_fromdata_init(privateKeyCtx.get()) != 1) {
            throw CryptoException();
        }
        if (EVP_PKEY_fromdata(privateKeyCtx.get(), &nodePrivKey, EVP_PKEY_KEYPAIR, params.get()) != 1) {
            throw CryptoException();
        }
    }

    void loadNodeKeys() {
        std::string serializedPubKey = core::getSetting("public key");
        if (serializedPubKey.empty()) {
            genNewKeyPair();
            return;
        }
        std::string serializedPrivKey = core::getSetting("private key");
        loadPubKey(serializedPubKey);
        loadPrivKey(serializedPrivKey);
        core::Node::thisNode->id = hashData(nodePubKey);
        core::Node::thisNode->publicKey = nodePubKey;
        storage::storeThisNode();
        BOOST_LOG_TRIVIAL(debug) << "Loaded node keys. ID: " << core::Node::thisNode->id->getHexRepresentation()
                                 << " Pub key: " << serializedPubKey;
    }

    std::unique_ptr<EVP_PKEY_CTX, decltype(&EVP_PKEY_CTX_free)> initDeriveKeyContext() {
        std::unique_ptr<EVP_PKEY_CTX, decltype(&EVP_PKEY_CTX_free)> ctx(
                EVP_PKEY_CTX_new(nodePrivKey, nullptr),
                EVP_PKEY_CTX_free);
        if (ctx == nullptr) {
            throw CryptoException();
        }
        if (EVP_PKEY_derive_init(ctx.get()) != 1) {
            throw CryptoException();
        }
        return ctx;
    }
}