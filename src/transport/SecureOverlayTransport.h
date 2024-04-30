#ifndef RINGSWARM_SECUREOVERLAYTRANSPORT_H
#define RINGSWARM_SECUREOVERLAYTRANSPORT_H

#include "Transport.h"

namespace RingSwarm::crypto {
class SymmetricCypher;
}

namespace RingSwarm::transport {
class SecureOverlayTransport : public Transport {
    std::unique_ptr<Transport> transport;
    std::unique_ptr<crypto::SymmetricCypher> cypher;
    std::vector<char> buffer;

    SecureOverlayTransport(std::unique_ptr<Transport> transport, std::unique_ptr<crypto::SymmetricCypher> cypher);

public:
    static async::Coroutine<std::unique_ptr<SecureOverlayTransport>> createClientSide(
            std::unique_ptr<Transport> transport, core::PublicKey &remotePublicKey);

    static async::Coroutine<std::unique_ptr<SecureOverlayTransport>, std::shared_ptr<core::PublicKey>> createServerSide(std::unique_ptr<Transport> transport);

    async::Coroutine<> rawRead(void *data, uint32_t size) override;

    void rawWrite(void *data, uint32_t len) override;

    void close() override;

    ~SecureOverlayTransport() override = default;
};
}// namespace RingSwarm::transport

#endif//RINGSWARM_SECUREOVERLAYTRANSPORT_H
