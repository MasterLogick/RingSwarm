#include "ServerHandler.h"

#include "../transport/ClosedTransportException.h"
#include "../transport/TransportException.h"

#include "RequestHeader.h"
#include "TooLargeMessageException.h"

#include <glog/logging.h>

#define MAX_REQUEST_LENGTH (1024 * 1024)

namespace RingSwarm::proto {
ServerHandler::ServerHandler(
    std::shared_ptr<transport::Transport> transport,
    core::Node &remote
)
    : transport(std::move(transport)), remote(remote), listening(false) {}

async::Coroutine<> ServerHandler::listen() {
    listening = true;
    while (listening) {
        try {
            RequestHeader rh{};
            ~co_await transport->rawRead(&rh, sizeof(rh));
            handlersKeepingLock.lock();
            for (const auto ref: finishedHandlers) {
                ~co_await async::Coroutine<>::resumeSuspended(
                    std::move(ref.get().handlerCoro)
                );
                pendingResponses.erase(ref.get().requestHeader.tag);
            }
            if (rh.requestLength > MAX_REQUEST_LENGTH ||
                rh.method >= CommandId_COMMAND_COUNT || rh.method == 0 ||
                pendingResponses.contains(rh.tag)) {
                handlersKeepingLock.unlock();
                // todo stop all handlers and exit
            }
            auto &serverRespState = pendingResponses[rh.tag];
            handlersKeepingLock.unlock();
            serverRespState.requestBuffer = transport::Buffer(rh.requestLength);
            serverRespState.requestHeader = rh;
            ~co_await transport->rawRead(
                serverRespState.requestBuffer.data,
                rh.requestLength
            );
            serverRespState.handlerCoro = handleRequest(serverRespState);
            async::Coroutine<>::scheduleCoroutineResume(
                serverRespState.handlerCoro.getHandle()
            );
        } catch (const transport::ClosedTransportException &e) {
            // todo
            break;
        }
    }
    for (auto &item: pendingResponses) {
        ~co_await async::Coroutine<>::resumeSuspended(
            std::move(item.second.handlerCoro)
        );
    }
}

async::Coroutine<>
ServerHandler::handleRequest(ServerResponseState &serverRespState) {
    co_await async::Coroutine<>::suspendThis([](auto h) {
        return std::noop_coroutine();
    });
    auto cid = serverRespState.requestHeader.method;
    ~co_await (this->*ServerHandler::Methods[cid])(serverRespState);

    writeLockWaitersLock.lock();
    if (writeLockWaiters.empty()) {
        writeLock = false;
    } else {
        async::Coroutine<>::scheduleCoroutineResume(writeLockWaiters.back());
        writeLockWaiters.pop_back();
    }
    writeLockWaitersLock.unlock();

    co_await async::Coroutine<>::suspendThis(
        [this, &serverRespState](std::coroutine_handle<async::Promise<>> f) {
            handlersKeepingLock.lock();
            finishedHandlers.emplace_back(serverRespState);
            handlersKeepingLock.unlock();
            return std::noop_coroutine();
        }
    );
}

async::Coroutine<>
ServerHandler::stubHandler(ServerResponseState &serverRespState) {
    VLOG(7) << "stub handler#m" << (int) serverRespState.requestHeader.method
            << "(t" << serverRespState.requestHeader.tag << ") l"
            << serverRespState.requestHeader.requestLength;
    co_return;
}

async::Coroutine<> ServerHandler::acquireWriteLock() {
    auto handle = co_await async::Coroutine<>::getThisCoroutineHandle();
    writeLockWaitersLock.lock();
    if (!writeLock) {
        writeLock = true;
        writeLockWaitersLock.unlock();
        co_return;
    }
    writeLockWaiters.push_back(handle);
    co_await async::Coroutine<>::suspendThis(
        [this](std::coroutine_handle<async::Promise<>> f) {
            writeLockWaitersLock.unlock();
            return std::noop_coroutine();
        }
    );
}
}// namespace RingSwarm::proto
