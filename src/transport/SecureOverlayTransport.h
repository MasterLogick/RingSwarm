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
    public:
        SecureOverlayTransport(Transport *transport, crypto::PublicKey *remotePublicKey);

        explicit SecureOverlayTransport(Transport *transport);

        void rawWrite(void *data, uint32_t len) override;

        void rawRead(void *buff, uint32_t len) override;

        void close() override;

        ~SecureOverlayTransport();
    };
}

#endif //RINGSWARM_SECUREOVERLAYTRANSPORT_H
