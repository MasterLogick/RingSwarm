#include "ClientTransport.h"
#include "../Assert.h"
#include "RequestHeader.h"
#include "ServerHandler.h"
#include <boost/log/trivial.hpp>

namespace RingSwarm::proto {
ClientTransport::ClientTransport(std::unique_ptr<transport::Transport> clientTransport)
    : transport(std::move(clientTransport)) {}

async::Coroutine<ResponseHeader> ClientTransport::sendRequest(CommandId commandId, RequestBuffer &req, uint32_t maxResponseSize) {
    req.assertFullyUsed();
    lock.lock();
    auto &requestState = reserveRequestState();
    requestState.maxResponseSize = maxResponseSize;
    auto *header = reinterpret_cast<RequestHeader *>(req.data);
    header->requestLength = req.len - sizeof(RequestHeader);
    header->method = commandId;
    header->tag = requestState.tag;
    co_await async::Coroutine<ResponseHeader>::suspendThis([&](auto h) {
        requestState.handle = h;
        transport->rawWrite(req.data, req.len);
        if (currentWaitForResponseCoroutine.getHandle() == nullptr) {
            currentWaitForResponseCoroutine = waitForAnyResponse();
        }
        lock.unlock();
        /*BOOST_LOG_TRIVIAL(trace) << "Sent request   |===> " << proto::ServerHandler::MethodNames[commandIndex]
                           << " " << req.len - sizeof(RequestHeader) << " bytes. Tag: " << ((int) tag);*/
        return std::noop_coroutine();
    });
    if (requestState.header.responseLen == 0) {
        lock.lock();
        if (!pendingRequests.empty()) {
            currentWaitForResponseCoroutine = waitForAnyResponse();
        } else {
            currentWaitForResponseCoroutine = async::Coroutine<>();
        }
        lock.unlock();
    }
    ResponseHeader rh = requestState.header;
    freeRequestState(requestState.tag);
    co_return rh;
}

async::Coroutine<transport::Buffer> ClientTransport::sendSmallRequest(CommandId commandId, RequestBuffer &req, uint32_t maxResponseSize) {
    auto rh = co_await sendRequest(commandId, req, maxResponseSize);
    auto *buff = new uint8_t[rh.responseLen];
    co_await rawRead(buff, rh.responseLen);
    co_return transport::Buffer(buff, rh.responseLen);
}

async::Coroutine<> ClientTransport::rawRead(void *data, uint32_t size) {
    Assert(size <= unreadResponseLength, "read request exceeded response size");
    co_await transport->rawRead(data, size);
    unreadResponseLength -= size;
    if (unreadResponseLength == 0) {
        lock.lock();
        if (!pendingRequests.empty()) {
            currentWaitForResponseCoroutine = waitForAnyResponse();
        } else {
            currentWaitForResponseCoroutine = async::Coroutine<>();
        }
        lock.unlock();
    }
}

async::Coroutine<> ClientTransport::waitForAnyResponse() {
    ResponseHeader rh{};
    transport->rawRead(&rh, sizeof(rh));
    lock.lock();
    if (pendingRequests.contains(rh.tag)) {
        auto &reqState = pendingRequests[rh.tag];
        lock.unlock();
        reqState.header = rh;
        unreadResponseLength = rh.responseLen;
        async::Coroutine<ResponseHeader>::scheduleCoroutineResume(reqState.handle);
    } else {
        //todo handle case
    }
    co_return;
}

ClientRequestState &ClientTransport::reserveRequestState() {
    lock.lock();
    for (uint16_t i = 0; i < UINT16_MAX; ++i) {
        if (!pendingRequests.contains(i)) {
            auto &a = pendingRequests[i];
            lock.unlock();
            return a;
        }
    }
    lock.unlock();
    //todo handle case
    return pendingRequests[0];
}

void ClientTransport::freeRequestState(uint16_t tag) {
    lock.lock();
    pendingRequests.erase(tag);
    lock.unlock();
}
}// namespace RingSwarm::proto