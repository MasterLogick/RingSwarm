#include "SecureOverlayTransport.h"

#include "../crypto/SymmetricalCypher.h"

#include <boost/algorithm/hex.hpp>

namespace RingSwarm::transport {
void SecureOverlayTransport::rawWrite(void *data, uint32_t len) {
    /*BOOST_LOG_TRIVIAL(trace) << "Secure overlay |===> "
                                 <<
       boost::algorithm::hex(std::string(static_cast<char *>(data), len));*/
    /*if (buffer.size() < len) {
        if (64 * 1024 < len) {
            char *dataCopy = static_cast<char *>(data);
            buffer.resize(64 * 1024);
            while (len > 0) {
                uint32_t bufferRemainder = std::min<uint32_t>(64 * 1024, len);
                cypher->encode(buffer.data(), dataCopy, bufferRemainder);
                transport->rawWrite(buffer.data(), bufferRemainder);
                dataCopy += bufferRemainder;
                len -= bufferRemainder;
            }
            return;
        }
        buffer.resize(len);
    }
    cypher->encode(buffer.data(), data, len);*/
    transport->rawWrite(buffer.data(), len);
}

void SecureOverlayTransport::close() {
    transport->close();
}

async::Coroutine<> SecureOverlayTransport::rawRead(void *data, uint32_t size) {
    ~co_await transport->rawRead(data, size);
    //    cypher->decode(data, data, size);
}

SecureOverlayTransport::SecureOverlayTransport(
    std::unique_ptr<Transport> transport,
    std::unique_ptr<crypto::SymmetricCypher> cypher
)
    : transport(std::move(transport)), cypher(std::move(cypher)) {}

async::Coroutine<std::unique_ptr<SecureOverlayTransport>>
SecureOverlayTransport::createClientSide(
    std::unique_ptr<Transport> transport,
    core::PublicKey &remotePublicKey
) {
    /*std::unique_ptr<EVP_PKEY, decltype(&EVP_PKEY_free)>
    keyPair(EVP_EC_gen(SN_secp256k1), EVP_PKEY_free); core::PublicKey rawPubKey;
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
    uint8_t iv[16];
    co_await transport->rawRead(iv, 16);
    auto cypher = std::make_unique<crypto::SymmetricCypher>(
            std::unique_ptr<EVP_PKEY_CTX,
    decltype(&EVP_PKEY_CTX_free)>(ctx.release(), EVP_PKEY_CTX_free),
            remotePublicKey, iv);*/
    co_return std::unique_ptr<SecureOverlayTransport>(
        new SecureOverlayTransport(std::move(transport), nullptr)
    );
}

async::Coroutine<
    std::unique_ptr<SecureOverlayTransport>,
    std::shared_ptr<core::PublicKey>>
SecureOverlayTransport::createServerSide(std::unique_ptr<Transport> transport) {
    /*core::PublicKey remotePublicKey;
    co_await transport->rawRead(remotePublicKey.data(), remotePublicKey.size());
    uint8_t iv[16];
    if (RAND_bytes(iv, 16) != 1) {
        throw crypto::CryptoException();
    }
    transport->rawWrite(iv, 16);
    auto cypher =
    std::make_unique<crypto::SymmetricCypher>(crypto::initDeriveKeyContext(),
    remotePublicKey, iv);*/
    co_return {
        std::unique_ptr<SecureOverlayTransport>(
            new SecureOverlayTransport(std::move(transport), nullptr)
        ),
        std::make_shared<core::PublicKey>()
    };
}
}// namespace RingSwarm::transport
