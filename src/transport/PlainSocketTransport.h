#ifndef RINGSWARM_PLAINSOCKETTRANSPORT_H
#define RINGSWARM_PLAINSOCKETTRANSPORT_H

#include "Transport.h"

#include "uvw/tcp.h"
#include <uvw/tcp.h>

#include <atomic>
#include <queue>
#include <string>

namespace RingSwarm::transport {
class PlainSocketTransport
    : public std::enable_shared_from_this<PlainSocketTransport>,
      public Transport {
    std::shared_ptr<uvw::tcp_handle> tcpHandler;
    transport::Buffer pending;
    char *readBufferData;
    uint32_t readBufferSize;
    uint32_t readBufferOffset;
    uvw::socket_address peerAddress;
    uvw::socket_address bindAddress;
    std::coroutine_handle<async::Promise<>> readPromiseHandle;
#ifndef NDEBUG
    std::atomic_flag writeFlag;
    std::atomic_flag readFlag;
#endif

    struct Private {};

public:
    PlainSocketTransport(
        const std::shared_ptr<uvw::tcp_handle> &handle,
        Private
    );

    static std::shared_ptr<PlainSocketTransport> Create();

    static std::shared_ptr<PlainSocketTransport>
    Create(const std::shared_ptr<uvw::tcp_handle> &handle);

    async::Coroutine<int> connect(std::string host, int port);

    async::Coroutine<> rawRead(void *data, uint32_t size) override;

    void rawWrite(void *data, uint32_t len) override;

    void close() override;

    ~PlainSocketTransport() override;

private:
    void setupHandler();

    void raiseClosedTransportException(std::coroutine_handle<async::Promise<>> h
    );

    void onClose();
};
}// namespace RingSwarm::transport

#endif// RINGSWARM_PLAINSOCKETTRANSPORT_H
