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
        BOOST_LOG_TRIVIAL(trace) << "Secure overlay |===> "
                                 << boost::algorithm::hex(std::string(static_cast<char *>(data), len));
        cypher->write(transport, data, len);
    }

    void SecureOverlayTransport::rawRead(void *buff, uint32_t len) {
        cypher->read(transport, buff, len);
        BOOST_LOG_TRIVIAL(trace) << "Secure overlay |<=== "
                                 << boost::algorithm::hex(std::string(static_cast<char *>(buff), len));
    }

    void SecureOverlayTransport::close() {
        transport->close();
    }

    SecureOverlayTransport::SecureOverlayTransport(
            transport::Transport *transport,
            core::PublicKey *remotePublicKey
    ) : transport(transport) {
        std::unique_ptr<EVP_PKEY, decltype(&EVP_PKEY_free)> keyPair(EVP_EC_gen(SN_secp256k1), EVP_PKEY_free);
        auto *rawPubKey = new core::PublicKey();
        size_t rawPubKeyLen = rawPubKey->size();
        if (EVP_PKEY_get_octet_string_param(keyPair.get(),
                                            OSSL_PKEY_PARAM_PUB_KEY,
                                            rawPubKey->data(),
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
        transport->rawWrite(rawPubKey, rawPubKey->size());
        uint8_t iv[16];
        transport->rawRead(iv, 16);
        cypher = new crypto::SymmetricCypher(std::move(ctx), remotePublicKey, iv);
    }

    SecureOverlayTransport::SecureOverlayTransport(transport::Transport *transport) : transport(transport) {
        uint8_t iv[16];
        if (RAND_bytes(iv, 16) != 1) {
            throw crypto::CryptoException();
        }
        transport->rawWrite(iv, 16);
        auto *remotePubKey = new core::PublicKey();
        transport->rawRead(remotePubKey->data(), remotePubKey->size());
        cypher = new crypto::SymmetricCypher(crypto::initDeriveKeyContext(), remotePubKey, iv);
    }

    SecureOverlayTransport::~SecureOverlayTransport() {
        delete cypher;
    }
}