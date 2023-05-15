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
    core::PublicKey *nodePubKey;
    EVP_PKEY *nodePrivKey;

    void genNewKeyPair() {
        crypto::KeyPair keyPair;

        std::string serializedPubKey;
        boost::algorithm::hex(keyPair.publicKey->begin(), keyPair.publicKey->end(),
                              std::back_inserter(serializedPubKey));
        BIGNUM *privateKey = nullptr;
        if (EVP_PKEY_get_bn_param(keyPair.privateKey, OSSL_PKEY_PARAM_PRIV_KEY, &privateKey) != 1) {
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
        nodePubKey = new core::PublicKey();
        boost::algorithm::unhex(serializedPubKey, nodePubKey->begin());
    }

    void loadPrivKey(std::string &serializedPrivKey) {
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