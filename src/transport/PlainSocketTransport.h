#ifndef RINGSWARM_PLAINSOCKETTRANSPORT_H
#define RINGSWARM_PLAINSOCKETTRANSPORT_H

#include <string>
#include "Transport.h"

namespace RingSwarm::transport {
    class PlainSocketTransport : public Transport {
        int sockFd;
    public:
        PlainSocketTransport(std::string &host, int port);
        explicit PlainSocketTransport(int sockFd);

        void rawWrite(void *data, uint32_t len) override;

        void rawRead(void *buff, uint32_t len) override;

        void close() override;

    };
}

#endif //RINGSWARM_PLAINSOCKETTRANSPORT_H
