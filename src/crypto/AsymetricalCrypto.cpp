#include "AsymetricalCrypto.h"
#include "CryptoException.h"
#include "HashCrypto.h"
#include "../core/Settings.h"
#include "../core/Node.h"
#include <openssl/param_build.h>
#include <openssl/core_names.h>
#include <openssl/objects.h>
#include <openssl/evp.h>
#include <openssl/ec.h>

namespace RingSwarm::crypto {
    std::vector<char> nodePubKey;
    EVP_PKEY *nodePrivKey;

    std::vector<char> signData(std::vector<char> data) {
        EVP_MD_CTX *mdctx;
        if (!(mdctx = EVP_MD_CTX_create())) throw CryptoException();
        if (1 != EVP_DigestSignInit(mdctx, nullptr, EVP_sha256(), nullptr, nodePrivKey)) {
            EVP_MD_CTX_destroy(mdctx);
            throw CryptoException();
        }
        if (1 != EVP_DigestSignUpdate(mdctx, data.data(), data.size())) {
            EVP_MD_CTX_destroy(mdctx);
            throw CryptoException();
        }
        size_t slen;
        if (1 != EVP_DigestSignFinal(mdctx, nullptr, &slen)) {
            EVP_MD_CTX_destroy(mdctx);
            throw CryptoException();
        }
        std::vector<char> sig(slen);
        if (1 != EVP_DigestSignFinal(mdctx, reinterpret_cast<unsigned char *>(sig.data()), &slen)) {
            EVP_MD_CTX_destroy(mdctx);
            throw CryptoException();
        }
        EVP_MD_CTX_destroy(mdctx);
        return sig;
    }

    bool verifyData(std::vector<char> &data, std::vector<char> &sig, std::vector<char> &pubKey) {
        OSSL_PARAM_BLD *paramBuild = OSSL_PARAM_BLD_new();
        if (paramBuild == nullptr) {
            throw CryptoException();
        }
        if (OSSL_PARAM_BLD_push_utf8_string(paramBuild,
                                            OSSL_PKEY_PARAM_GROUP_NAME, SN_secp256k1, 0) != 1) {
            OSSL_PARAM_BLD_free(paramBuild);
            throw CryptoException();
        }
        if (OSSL_PARAM_BLD_push_octet_string(paramBuild, OSSL_PKEY_PARAM_PUB_KEY,
                                             pubKey.data(), pubKey.size()) != 1) {
            OSSL_PARAM_BLD_free(paramBuild);
            throw CryptoException();
        }
        OSSL_PARAM *params = OSSL_PARAM_BLD_to_param(paramBuild);
        if (params == nullptr) {
            OSSL_PARAM_BLD_free(paramBuild);
            throw CryptoException();
        }
        EVP_PKEY_CTX *publicKeyCtx = EVP_PKEY_CTX_new_from_name(nullptr, "EC", nullptr);
        if (publicKeyCtx == nullptr) {
            OSSL_PARAM_BLD_free(paramBuild);
            OSSL_PARAM_free(params);
            throw CryptoException();
        }
        if (EVP_PKEY_fromdata_init(publicKeyCtx) <= 0) {
            OSSL_PARAM_BLD_free(paramBuild);
            OSSL_PARAM_free(params);
            EVP_PKEY_CTX_free(publicKeyCtx);
            throw CryptoException();
        }
        EVP_PKEY *publicKey = nullptr;
        if (EVP_PKEY_fromdata(publicKeyCtx, &publicKey,
                              EVP_PKEY_PUBLIC_KEY, params) <= 0) {
            OSSL_PARAM_BLD_free(paramBuild);
            OSSL_PARAM_free(params);
            EVP_PKEY_CTX_free(publicKeyCtx);
            throw CryptoException();
        }
        OSSL_PARAM_BLD_free(paramBuild);
        OSSL_PARAM_free(params);
        EVP_PKEY_CTX_free(publicKeyCtx);


        auto *mdctx = EVP_MD_CTX_create();
        if (EVP_DigestVerifyInit(mdctx, nullptr, EVP_sha256(), nullptr, publicKey) != 1) {
            EVP_MD_CTX_destroy(mdctx);
            EVP_PKEY_free(publicKey);
            throw CryptoException();
        }
        if (EVP_DigestVerifyUpdate(mdctx, data.data(), data.size()) != 1) {
            EVP_MD_CTX_destroy(mdctx);
            EVP_PKEY_free(publicKey);
            throw CryptoException();
        }
        if (EVP_DigestVerifyFinal(mdctx, reinterpret_cast<const unsigned char *>(pubKey.data()), pubKey.size()) == 1) {
            EVP_MD_CTX_destroy(mdctx);
            EVP_PKEY_free(publicKey);
            return true;
        } else {
            EVP_MD_CTX_destroy(mdctx);
            EVP_PKEY_free(publicKey);
            return false;
        }
    }

    void genNewKeyPair() {
        auto *keyPair = EVP_EC_gen(SN_secp256k1);

        size_t rawPubKeyLen = 0;
        if (EVP_PKEY_get_octet_string_param(keyPair, OSSL_PKEY_PARAM_PUB_KEY, nullptr, 0, &rawPubKeyLen) != 1) {
            EVP_PKEY_free(keyPair);
            throw CryptoException();
        }
        std::vector<char> rawPubKey(rawPubKeyLen);
        if (EVP_PKEY_get_octet_string_param(keyPair,
                                            OSSL_PKEY_PARAM_PUB_KEY,
                                            reinterpret_cast<unsigned char *>(rawPubKey.data()),
                                            rawPubKeyLen,
                                            &rawPubKeyLen) != 1) {
            EVP_PKEY_free(keyPair);
            throw CryptoException();
        }
        size_t serializedPubKeyLen = (rawPubKeyLen + 2) / 3 * 4 + 1;
        std::string serializedPubKey(serializedPubKeyLen, '0');
        if (EVP_EncodeBlock(reinterpret_cast<unsigned char *>(serializedPubKey.data()),
                            reinterpret_cast<const unsigned char *>(rawPubKey.data()),
                            rawPubKeyLen) != serializedPubKeyLen - 1) {
            EVP_PKEY_free(keyPair);
            throw CryptoException();
        }

        BIGNUM *privateKey = nullptr;
        if (EVP_PKEY_get_bn_param(keyPair, OSSL_PKEY_PARAM_PRIV_KEY, &privateKey) != 1) {
            EVP_PKEY_free(keyPair);
            throw CryptoException();
        }
        char *serializedPrivKey = BN_bn2hex(privateKey);
        core::setSetting("pubKey", serializedPubKey.c_str());
        core::setSetting("privKey", serializedPrivKey);
        OPENSSL_free(serializedPrivKey);
        BN_free(privateKey);
        EVP_PKEY_free(keyPair);
        loadNodeKeys();
    }

    void loadPubKey(std::string &serializedPubKey) {
        size_t realLen = serializedPubKey.length() / 4 * 3;
        auto *decodedPubKey = new uint8_t[realLen];
        if (EVP_DecodeBlock(decodedPubKey,
                            reinterpret_cast<const unsigned char *>(serializedPubKey.c_str()),
                            serializedPubKey.length()) < 0) {
            delete[] decodedPubKey;
            throw CryptoException();
        }
        if (serializedPubKey.ends_with("==")) {
            realLen -= 2;
        }
        if (serializedPubKey.ends_with("=")) {
            realLen -= 1;
        }
        nodePubKey = std::vector<char>(decodedPubKey, decodedPubKey + realLen);
    }

    void loadPrivKey(std::string &serializedPrivKey) {
        OSSL_PARAM_BLD *paramBuild = OSSL_PARAM_BLD_new();
        if (paramBuild == nullptr) {
            throw CryptoException();
        }
        if (OSSL_PARAM_BLD_push_utf8_string(paramBuild,
                                            OSSL_PKEY_PARAM_GROUP_NAME,
                                            SN_secp256k1,
                                            0) != 1) {
            OSSL_PARAM_BLD_free(paramBuild);
            throw CryptoException();
        }
        BIGNUM *privKeyBn = nullptr;
        if (BN_hex2bn(&privKeyBn, serializedPrivKey.c_str()) == 0) {
            OSSL_PARAM_BLD_free(paramBuild);
            throw CryptoException();
        }
        if (OSSL_PARAM_BLD_push_BN(paramBuild,
                                   OSSL_PKEY_PARAM_PRIV_KEY,
                                   privKeyBn) != 1) {
            OSSL_PARAM_BLD_free(paramBuild);
            BN_free(privKeyBn);
            throw CryptoException();
        }
        OSSL_PARAM *params = OSSL_PARAM_BLD_to_param(paramBuild);
        if (params == nullptr) {
            OSSL_PARAM_BLD_free(paramBuild);
            BN_free(privKeyBn);
            throw CryptoException();
        }
        EVP_PKEY_CTX *privateKeyCtx = EVP_PKEY_CTX_new_from_name(nullptr, "EC", nullptr);
        if (privateKeyCtx == nullptr) {
            OSSL_PARAM_BLD_free(paramBuild);
            OSSL_PARAM_free(params);
            BN_free(privKeyBn);
            throw CryptoException();
        }
        if (EVP_PKEY_fromdata_init(privateKeyCtx) != 1) {
            OSSL_PARAM_BLD_free(paramBuild);
            OSSL_PARAM_free(params);
            EVP_PKEY_CTX_free(privateKeyCtx);
            BN_free(privKeyBn);
            throw CryptoException();
        }
        if (EVP_PKEY_fromdata(privateKeyCtx, &nodePrivKey, EVP_PKEY_KEYPAIR, params) != 1) {
            OSSL_PARAM_BLD_free(paramBuild);
            OSSL_PARAM_free(params);
            EVP_PKEY_CTX_free(privateKeyCtx);
            BN_free(privKeyBn);
            throw CryptoException();
        }
        OSSL_PARAM_BLD_free(paramBuild);
        OSSL_PARAM_free(params);
        EVP_PKEY_CTX_free(privateKeyCtx);
        BN_free(privKeyBn);
    }

    void loadNodeKeys() {
        std::string serializedPubKey = core::getSetting("pubKey");
        if (serializedPubKey.empty()) {
            genNewKeyPair();
            return;
        }
        std::string serializedPrivKey = core::getSetting("privKey");
        loadPubKey(serializedPubKey);
        loadPrivKey(serializedPrivKey);
        core::Node::thisNode->id = hashData(reinterpret_cast<const uint8_t *>(nodePubKey.data()), nodePubKey.size());
        core::Node::thisNode->publicKey = nodePubKey;
    }
}