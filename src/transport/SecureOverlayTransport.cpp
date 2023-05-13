#include <openssl/objects.h>
#include <openssl/evp.h>
#include <openssl/ec.h>
#include <openssl/core_names.h>
#include "SecureOverlayTransport.h"
#include "../crypto/AsymmetricalCrypto.h"
#include "../crypto/CryptoException.h"
#include "../crypto/SymmetricalCrypher.h"
#include <openssl/rand.h>
#include <boost/log/trivial.hpp>
#include <boost/algorithm/hex.hpp>

namespace RingSwarm::transport {
    void SecureOverlayTransport::rawWrite(void *data, uint32_t len) {
        BOOST_LOG_TRIVIAL(trace) << "Secure overlay sent: "
                                 << boost::algorithm::hex(std::string(static_cast<char *>(data), len));
        cypher->write(transport, data, len);
    }

    void SecureOverlayTransport::rawRead(void *buff, uint32_t len) {
        cypher->read(transport, buff, len);
        BOOST_LOG_TRIVIAL(trace) << "Secure overlay read: "
                                 << boost::algorithm::hex(std::string(static_cast<char *>(buff), len));
    }

    void SecureOverlayTransport::close() {
        transport->close();
    }

    SecureOverlayTransport::SecureOverlayTransport(transport::Transport *transport,
                                                   std::vector<char> &remotePublicKey) : transport(transport) {
        auto *keyPair = EVP_EC_gen(SN_secp256k1);
        size_t rawPubKeyLen = 0;
        if (EVP_PKEY_get_octet_string_param(keyPair, OSSL_PKEY_PARAM_PUB_KEY, nullptr, 0, &rawPubKeyLen) != 1) {
            EVP_PKEY_free(keyPair);
            throw crypto::CryptoException();
        }
        if (rawPubKeyLen != RAW_NODE_PUBLIC_KEY_LENGTH) {
            EVP_PKEY_free(keyPair);
            throw crypto::CryptoException();
        }
        uint8_t rawPubKey[RAW_NODE_PUBLIC_KEY_LENGTH];

        if (EVP_PKEY_get_octet_string_param(keyPair,
                                            OSSL_PKEY_PARAM_PUB_KEY,
                                            rawPubKey,
                                            rawPubKeyLen,
                                            &rawPubKeyLen) != 1) {
            EVP_PKEY_free(keyPair);
            throw crypto::CryptoException();
        }
        transport->rawWrite(rawPubKey, rawPubKeyLen);
        uint8_t iv[16];
        transport->rawRead(iv, 16);
        cypher = new crypto::SymmetricCypher(keyPair, remotePublicKey, iv);
    }

    SecureOverlayTransport::SecureOverlayTransport(transport::Transport *transport) : transport(transport) {
        uint8_t iv[16];
        if (RAND_bytes(iv, 16) != 1) {
            throw crypto::CryptoException();
        }
        transport->rawWrite(iv, 16);
        std::vector<char> remotePubKey(RAW_NODE_PUBLIC_KEY_LENGTH);
        transport->rawRead(remotePubKey.data(), RAW_NODE_PUBLIC_KEY_LENGTH);
        cypher = new crypto::SymmetricCypher(remotePubKey, iv);
    }

    SecureOverlayTransport::~SecureOverlayTransport() {
        delete cypher;
    }
}