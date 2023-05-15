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

    SymmetricCypher::SymmetricCypher(std::unique_ptr<EVP_PKEY_CTX, decltype(&EVP_PKEY_CTX_free)> ctx,
                                     crypto::PublicKey *serializedPublicKey, const uint8_t *iv) :
            cipher(nullptr, EVP_CIPHER_CTX_free), decipher(nullptr, EVP_CIPHER_CTX_free) {
        std::unique_ptr<EVP_PKEY_CTX, decltype(&EVP_PKEY_CTX_free)> publicKeyCtx(
                EVP_PKEY_CTX_new_from_name(nullptr, "EC", nullptr),
                EVP_PKEY_CTX_free);
        if (publicKeyCtx == nullptr) {
            throw CryptoException();
        }
        if (EVP_PKEY_fromdata_init(publicKeyCtx.get()) != 1) {
            throw CryptoException();
        }
        OSSL_PARAM params[] = {
                OSSL_PARAM_utf8_string(OSSL_PKEY_PARAM_GROUP_NAME, (void *) SN_secp256k1, strlen(SN_secp256k1)),
                OSSL_PARAM_octet_string(OSSL_PKEY_PARAM_PUB_KEY, serializedPublicKey->data(),
                                        serializedPublicKey->size()),
                OSSL_PARAM_END
        };
        EVP_PKEY *ptr = nullptr;
        if (EVP_PKEY_fromdata(publicKeyCtx.get(), &ptr, EVP_PKEY_PUBLIC_KEY, params) != 1) {
            throw CryptoException();
        }
        std::unique_ptr<EVP_PKEY, decltype(&EVP_PKEY_free)> pubKey(ptr, EVP_PKEY_free);
        if (EVP_PKEY_derive_set_peer(ctx.get(), pubKey.get()) != 1) {
            throw CryptoException();
        }
        size_t secretSize = 0;
        if (EVP_PKEY_derive(ctx.get(), nullptr, &secretSize) != 1) {
            throw CryptoException();
        }
        uint8_t secret[secretSize];
        if (EVP_PKEY_derive(ctx.get(), secret, &secretSize) != 1) {
            std::cout << ERR_error_string(ERR_get_error(), nullptr) << std::endl;
            throw CryptoException();
        }
        BOOST_LOG_TRIVIAL(trace) << "Shared secret: "
                                 << boost::algorithm::hex(
                                         std::string(reinterpret_cast<const char *>(secret), secretSize));
        cipher.reset(EVP_CIPHER_CTX_new());
        decipher.reset(EVP_CIPHER_CTX_new());
        if (EVP_EncryptInit(cipher.get(), EVP_aes_256_ctr(), secret, iv) != 1) {
            throw CryptoException();
        }
        if (EVP_DecryptInit(decipher.get(), EVP_aes_256_ctr(), secret, iv) != 1) {
            throw CryptoException();
        }
    }


    void SymmetricCypher::write(RingSwarm::transport::Transport *transport, void *data, uint32_t len) {
        uint8_t buff[8192];
        int size;
        while (len > 8192) {
            if (EVP_EncryptUpdate(cipher.get(), buff, &size, static_cast<const uint8_t *>(data), 8192) != 1) {
                throw crypto::CryptoException();
            }
            if (size != 8192) {
                throw crypto::CryptoException();
            }
            transport->rawWrite(buff, 8192);
            len -= 8192;
            data = static_cast<uint8_t *>(data) + 8192;
        }
        if (EVP_EncryptUpdate(cipher.get(), buff, &size, static_cast<const uint8_t *>(data), len) != 1) {
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
            if (EVP_DecryptUpdate(decipher.get(), static_cast<uint8_t *>(data), &size, buff, 8192) != 1) {
                throw crypto::CryptoException();
            }
            if (size != 8192) {
                throw crypto::CryptoException();
            }
            len -= 8192;
            data = static_cast<uint8_t *>(data) + 8192;
        }
        transport->rawRead(buff, len);
        if (EVP_DecryptUpdate(decipher.get(), static_cast<uint8_t *>(data), &size, buff, len) != 1) {
            throw crypto::CryptoException();
        }
        if (size != len) {
            throw crypto::CryptoException();
        }
    }
}