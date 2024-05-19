#ifndef RINGSWARM_TRANSPORTCLIENTSIDEWRAPPER_H
#define RINGSWARM_TRANSPORTCLIENTSIDEWRAPPER_H

#include "../transport/TransportWrapper.h"

#include "ClientRequestState.h"
#include "CommandId.h"
#include "RequestBuffer.h"
#include "ResponseHeader.h"

namespace RingSwarm::proto {
class ClientTransport {
    std::shared_ptr<transport::Transport> transport;
    std::map<uint16_t, ClientRequestState> pendingRequests;
    std::mutex lock;
    uint32_t unreadResponseLength;
    uint16_t unreadResponseTag;
    async::Coroutine<> currentWaitForResponseCoroutine;

    ClientRequestState &reserveRequestState();

    async::Coroutine<> waitForAnyResponse();

public:
    explicit ClientTransport(
        std::shared_ptr<transport::Transport> clientTransport
    );

    async::Coroutine<ResponseHeader> sendRequest(
        CommandId commandId,
        RequestBuffer &req,
        uint32_t maxResponseSize
    );

    async::Coroutine<transport::Buffer> sendSmallRequest(
        CommandId commandId,
        RequestBuffer &req,
        uint32_t maxResponseSize
    );

    async::Coroutine<> rawRead(void *data, uint32_t size);
};
}// namespace RingSwarm::proto

#endif// RINGSWARM_TRANSPORTCLIENTSIDEWRAPPER_H
