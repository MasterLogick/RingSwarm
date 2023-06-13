#include <openssl/objects.h>
#include <openssl/evp.h>
#include <openssl/ec.h>
#include <openssl/core_names.h>
#include "SecureOverlayTransport.h"
#include "../crypto/CryptoException.h"
#include "../crypto/SymmetricalCrypher.h"
#include <openssl/rand.h>
#include <boost/log/trivial.hpp>
#include <boost/algorithm/hex.hpp>

namespace RingSwarm::transport {
    void SecureOverlayTransport::rawWrite(void *data, uint32_t len) {
        /*BOOST_LOG_TRIVIAL(trace) << "Secure overlay |===> "
                                 << boost::algorithm::hex(std::string(static_cast<char *>(data), len));*/
        cypher->encode(data, len);
        transport->rawWrite(data, len);
    }

    void SecureOverlayTransport::close() {
        transport->close();
    }

    SecureOverlayTransport::~SecureOverlayTransport() {
        delete cypher;
    }

    std::shared_ptr<async::Future<void>> SecureOverlayTransport::rawRead(void *data, uint32_t size) {
        return transport->rawRead(data, size)->then([this, data, size]() {
            cypher->decode(static_cast<uint8_t *>(data), size);
            /*BOOST_LOG_TRIVIAL(trace) << "Secure overlay |<=== "
                                     << boost::algorithm::hex(std::string(static_cast<const char *>(data), size));*/
            return data;
        });
    }

    SecureOverlayTransport::SecureOverlayTransport(Transport *transport, crypto::SymmetricCypher *cypher) :
            transport(transport), cypher(cypher) {

    }

    std::shared_ptr<async::Future<SecureOverlayTransport *>> SecureOverlayTransport::createClientSide(
            Transport *transport, core::PublicKey *remotePublicKey) {
        std::unique_ptr<EVP_PKEY, decltype(&EVP_PKEY_free)> keyPair(EVP_EC_gen(SN_secp256k1), EVP_PKEY_free);
        core::PublicKey rawPubKey;
        size_t rawPubKeyLen = rawPubKey.size();
        if (EVP_PKEY_get_octet_string_param(keyPair.get(),
                                            OSSL_PKEY_PARAM_PUB_KEY,
                                            rawPubKey.data(),
                                            rawPubKeyLen,
                                            &rawPubKeyLen) != 1) {
            throw crypto::CryptoException();
        }
        std::unique_ptr<EVP_PKEY_CTX, decltype(&EVP_PKEY_CTX_free)> ctx(
                EVP_PKEY_CTX_new(keyPair.get(), nullptr),
                EVP_PKEY_CTX_free);
        if (ctx == nullptr) {
            throw crypto::CryptoException();
        }
        if (EVP_PKEY_derive_init(ctx.get()) != 1) {
            throw crypto::CryptoException();
        }
        transport->rawWrite(rawPubKey.data(), rawPubKey.size());
        //todo optimize
        auto *iv = new uint8_t[16];
        return transport->rawRead(iv, 16)->then<SecureOverlayTransport *>([transport, ctx = ctx.release(),
                                                                                  remotePublicKey, iv]() {
            auto *cypher = new crypto::SymmetricCypher(
                    std::unique_ptr<EVP_PKEY_CTX, decltype(&EVP_PKEY_CTX_free)>(ctx, EVP_PKEY_CTX_free),
                    remotePublicKey, iv);
            delete[] iv;
            return new SecureOverlayTransport(transport, cypher);
        });
    }

    std::shared_ptr<async::Future<SecureOverlayTransport *>>
    SecureOverlayTransport::createServerSide(Transport *transport) {
        auto *remotePublicKey = new core::PublicKey();
        return transport->rawRead(remotePublicKey->data(), remotePublicKey->size())->
                then<SecureOverlayTransport *>([transport, remotePublicKey]() {
            uint8_t iv[16];
            if (RAND_bytes(iv, 16) != 1) {
                throw crypto::CryptoException();
            }
            transport->rawWrite(iv, 16);
            auto *cypher = new crypto::SymmetricCypher(crypto::initDeriveKeyContext(), remotePublicKey, iv);
            return new SecureOverlayTransport(transport, cypher);
        });
    }
}