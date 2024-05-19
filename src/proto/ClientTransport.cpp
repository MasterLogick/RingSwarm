#include "ClientTransport.h"

#include "../Assert.h"

#include "RequestHeader.h"
#include "ServerHandler.h"
#include "UnknownResponseException.h"

#include <boost/log/trivial.hpp>

#include <utility>

namespace RingSwarm::proto {
ClientTransport::ClientTransport(
    std::shared_ptr<transport::Transport> clientTransport
)
    : transport(std::move(clientTransport)) {}

async::Coroutine<ResponseHeader> ClientTransport::sendRequest(
    CommandId commandId,
    RequestBuffer &req,
    uint32_t maxResponseSize
) {
    req.assertFullyUsed();
    auto &requestState = reserveRequestState();
    requestState.maxResponseSize = maxResponseSize;
    auto *header = reinterpret_cast<RequestHeader *>(req.data);
    header->requestLength = req.len - sizeof(RequestHeader);
    header->method = commandId;
    header->tag = requestState.tag;
    co_await async::Coroutine<ResponseHeader>::suspendThis([&](auto h) {
        requestState.handle = h;
        transport->rawWrite(req.data, req.len);
        if (pendingRequests.size() == 1) {
            Assert(
                currentWaitForResponseCoroutine == nullptr,
                "previous currentWaitForResponseCoroutine must be freed below after exit from suspend"
            );
            currentWaitForResponseCoroutine = waitForAnyResponse();
        }
        lock.unlock();
        /*BOOST_LOG_TRIVIAL(trace) << "Sent request   |===> " <<
           proto::ServerHandler::MethodNames[commandIndex]
                           << " " << req.len - sizeof(RequestHeader) << " bytes.
           Tag: " << ((int) tag);*/
        return std::noop_coroutine();
    });
    lock.lock();
    ~co_await async::Coroutine<>::resumeSuspended(
        std::move(currentWaitForResponseCoroutine)
    );
    if (requestState.header.responseLen == 0) {
        pendingRequests.erase(requestState.tag);
        if (!pendingRequests.empty()) {
            currentWaitForResponseCoroutine = waitForAnyResponse();
        } else {
            currentWaitForResponseCoroutine = async::Coroutine<>();
        }
    }
    lock.unlock();
    ResponseHeader rh = requestState.header;
    co_return rh;
}

async::Coroutine<transport::Buffer> ClientTransport::sendSmallRequest(
    CommandId commandId,
    RequestBuffer &req,
    uint32_t maxResponseSize
) {
    ResponseHeader rh = co_await sendRequest(commandId, req, maxResponseSize);
    transport::Buffer resp(rh.responseLen);
    if (rh.responseLen != 0) {
        ~co_await rawRead(resp.data, rh.responseLen);
    }
    co_return resp;
}

async::Coroutine<> ClientTransport::rawRead(void *data, uint32_t size) {
    Assert(
        size <= unreadResponseLength,
        "read request must not exceed remainder size"
    );
    Assert(unreadResponseLength > 0, "there must be some unread remainder");
    ~co_await transport->rawRead(data, size);
    unreadResponseLength -= size;
    if (unreadResponseLength == 0) {
        lock.lock();
        ~co_await async::Coroutine<>::resumeSuspended(
            std::move(currentWaitForResponseCoroutine)
        );
        pendingRequests.erase(unreadResponseTag);
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
    ~co_await transport->rawRead(&rh, sizeof(rh));
    auto iter = pendingRequests.find(rh.tag);
    if (iter == pendingRequests.end()) {
        throw UnknownResponseException();
    }
    auto &reqState = iter->second;
    reqState.header = rh;
    unreadResponseLength = rh.responseLen;
    unreadResponseTag = rh.tag;
    co_await async::Coroutine<>::suspendThis(
        [reqState](std::coroutine_handle<async::Promise<>> f) {
            async::Coroutine<ResponseHeader>::scheduleCoroutineResume(
                reqState.handle
            );
            return std::noop_coroutine();
        }
    );
}

ClientRequestState &ClientTransport::reserveRequestState() {
    do {
        while (pendingRequests.size() == UINT16_MAX) {}
        lock.lock();
        for (uint16_t i = 0; i < UINT16_MAX; ++i) {
            if (!pendingRequests.contains(i)) {
                auto &a = pendingRequests[i];
                a.tag = i;
                return a;
            }
        }
        lock.unlock();
    } while (true);
}
}// namespace RingSwarm::proto
