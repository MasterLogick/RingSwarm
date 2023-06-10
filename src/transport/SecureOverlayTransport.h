#ifndef RINGSWARM_SECUREOVERLAYTRANSPORT_H
#define RINGSWARM_SECUREOVERLAYTRANSPORT_H

#include "Transport.h"

namespace RingSwarm::crypto {
    class SymmetricCypher;
}
namespace RingSwarm::transport {
    class SecureOverlayTransport : public Transport {
        Transport *transport;
        crypto::SymmetricCypher *cypher;

        SecureOverlayTransport(Transport *transport, crypto::SymmetricCypher *cypher);

    public:
        static std::shared_ptr<async::Future<SecureOverlayTransport *>> createClientSide(
                Transport *transport, core::PublicKey *remotePublicKey);

        static std::shared_ptr<async::Future<SecureOverlayTransport *>> createServerSide(Transport *transport);

        std::shared_ptr<async::Future<void>> rawRead(void* data, uint32_t size) override;

        void rawWrite(void *data, uint32_t len) override;

        void close() override;

        ~SecureOverlayTransport() override;
    };
}

#endif //RINGSWARM_SECUREOVERLAYTRANSPORT_H
