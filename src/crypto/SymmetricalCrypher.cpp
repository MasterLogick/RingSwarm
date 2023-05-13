#include <openssl/evp.h>
#include <openssl/param_build.h>
#include <openssl/core_names.h>
#include "SymmetricalCrypher.h"
#include "CryptoException.h"
#include "AsymmetricalCrypto.h"
#include <iostream>
#include <openssl/err.h>
#include <boost/log/trivial.hpp>
#include <boost/algorithm/hex.hpp>

namespace RingSwarm::crypto {

    SymmetricCypher::SymmetricCypher(EVP_PKEY *privateKey, std::vector<char> &remotePublicKey, uint8_t *iv) {
        auto *ctx = EVP_PKEY_CTX_new(privateKey, nullptr);
        if (ctx == nullptr) {
            throw CryptoException();
        }
        if (EVP_PKEY_derive_init(ctx) != 1) {
            EVP_PKEY_CTX_free(ctx);
            throw CryptoException();
        }
        initCypher(ctx, remotePublicKey, iv);
        EVP_PKEY_CTX_free(ctx);
    }

    SymmetricCypher::SymmetricCypher(std::vector<char> &remotePublicKey, uint8_t *iv) {
        auto *ctx = initDeriveKeyContext();
        initCypher(ctx, remotePublicKey, iv);
        EVP_PKEY_CTX_free(ctx);
    }

    void SymmetricCypher::initCypher(EVP_PKEY_CTX *ctx, std::vector<char> &serializedPubKey, uint8_t *iv) {
        EVP_PKEY_CTX *publicKeyCtx = EVP_PKEY_CTX_new_from_name(nullptr, "EC", nullptr);
        if (publicKeyCtx == nullptr) {
            throw CryptoException();
        }
        if (EVP_PKEY_fromdata_init(publicKeyCtx) != 1) {
            EVP_PKEY_CTX_free(publicKeyCtx);
            throw CryptoException();
        }
        OSSL_PARAM params[] = {
                OSSL_PARAM_utf8_string(OSSL_PKEY_PARAM_GROUP_NAME, (void *) SN_secp256k1, strlen(SN_secp256k1)),
                OSSL_PARAM_octet_string(OSSL_PKEY_PARAM_PUB_KEY, serializedPubKey.data(), serializedPubKey.size()),
                OSSL_PARAM_END
        };
        EVP_PKEY *pubKey = nullptr;
        if (EVP_PKEY_fromdata(publicKeyCtx, &pubKey, EVP_PKEY_PUBLIC_KEY, params) != 1) {
            EVP_PKEY_CTX_free(publicKeyCtx);
            std::cout << ERR_error_string(ERR_get_error(), nullptr) << std::endl;
            throw CryptoException();
        }
        EVP_PKEY_CTX_free(publicKeyCtx);

        if (EVP_PKEY_derive_set_peer(ctx, pubKey) != 1) {
            EVP_PKEY_free(pubKey);
            std::cout << ERR_error_string(ERR_get_error(), nullptr) << std::endl;
            throw CryptoException();
        }
        size_t secretSize;
        if (EVP_PKEY_derive(ctx, nullptr, &secretSize) != 1) {
            EVP_PKEY_free(pubKey);
            throw CryptoException();
        }
        if (secretSize != 32) {
            EVP_PKEY_free(pubKey);
            throw CryptoException();
        }
        uint8_t secret[secretSize];
        if (EVP_PKEY_derive(ctx, secret, &secretSize) != 1) {
            EVP_PKEY_free(pubKey);
            throw CryptoException();
        }
        EVP_PKEY_free(pubKey);
        BOOST_LOG_TRIVIAL(trace) << "Shared secret: "
                                 << boost::algorithm::hex(
                                         std::string(reinterpret_cast<const char *>(secret), secretSize));
        cipher = EVP_CIPHER_CTX_new();
        decipher = EVP_CIPHER_CTX_new();

        if (EVP_EncryptInit(cipher, EVP_aes_256_ctr(), secret, reinterpret_cast<const unsigned char *>(iv)) != 1) {
            EVP_CIPHER_CTX_free(cipher);
            EVP_CIPHER_CTX_free(decipher);
        }
        if (EVP_DecryptInit(decipher, EVP_aes_256_ctr(), secret, reinterpret_cast<const unsigned char *>(iv)) != 1) {
            EVP_CIPHER_CTX_free(cipher);
            EVP_CIPHER_CTX_free(decipher);
        }
    }

    void SymmetricCypher::write(RingSwarm::transport::Transport *transport, void *data, uint32_t len) {
        uint8_t buff[8192];
        int size;
        while (len > 8192) {
            if (EVP_EncryptUpdate(cipher, buff, &size, static_cast<const uint8_t *>(data), 8192) != 1) {
                throw crypto::CryptoException();
            }
            if (size != 8192) {
                throw crypto::CryptoException();
            }
            transport->rawWrite(buff, 8192);
            len -= 8192;
            data = static_cast<uint8_t *>(data) + 8192;
        }
        if (EVP_EncryptUpdate(cipher, buff, &size, static_cast<const uint8_t *>(data), len) != 1) {
            throw crypto::CryptoException();
        }
        if (size != len) {
            throw crypto::CryptoException();
        }
        transport->rawWrite(buff, len);
    }

    void SymmetricCypher::read(transport::Transport *transport, void *data, uint32_t len) {
        uint8_t buff[8192];
        int size;
        while (len > 8192) {
            transport->rawRead(buff, 8192);
            if (EVP_DecryptUpdate(decipher, static_cast<uint8_t *>(data), &size, buff, 8192) != 1) {
                throw crypto::CryptoException();
            }
            if (size != 8192) {
                throw crypto::CryptoException();
            }
            len -= 8192;
            data = static_cast<uint8_t *>(data) + 8192;
        }
        transport->rawRead(buff, len);
        if (EVP_DecryptUpdate(decipher, static_cast<uint8_t *>(data), &size, buff, len) != 1) {
            throw crypto::CryptoException();
        }
        if (size != len) {
            throw crypto::CryptoException();
        }
    }

    SymmetricCypher::~SymmetricCypher() {
        EVP_CIPHER_CTX_free(cipher);
        EVP_CIPHER_CTX_free(decipher);
    }
}