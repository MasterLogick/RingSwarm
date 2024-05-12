#include "ServerHandler.h"

#include "../transport/ClosedTransportException.h"
#include "../transport/TransportException.h"

#include "RequestHeader.h"
#include "TooLargeMessageException.h"

#include <boost/log/trivial.hpp>

#define MAX_REQUEST_LENGTH (1024 * 1024)

namespace RingSwarm::proto {
ServerHandler::ServerHandler(
    std::unique_ptr<transport::Transport> transport,
    core::Node &remote
)
    : transport(std::move(transport)), remote(remote), listening(false) {}

async::Coroutine<> ServerHandler::listen() {
    listening = true;
    while (listening) {
        try {
            RequestHeader rh{};
            ~co_await transport->rawRead(&rh, sizeof(rh));
            lock.lock();
            for (const auto ref: finishedHandlers) {
                ref.get().handlerCoro.getHandle().promise().check();
                pendingResponses.erase(ref.get().requestHeader.tag);
            }
            if (rh.requestLength > MAX_REQUEST_LENGTH ||
                rh.method >= CommandId_COMMAND_COUNT || rh.method == 0 ||
                pendingResponses.contains(rh.tag)) {
                lock.unlock();
                // todo stop all handlers and exit
            }
            auto &serverRespState = pendingResponses[rh.tag];
            lock.unlock();
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
        } catch (const transport::ClosedTransportException &e) { break; }
    }
    for (const auto &item: pendingResponses) {
        ~co_await (async::Coroutine<> &&) std::move(item.second.handlerCoro);
    }
}

async::Coroutine<>
ServerHandler::handleRequest(ServerResponseState &serverRespState) {
    co_await async::Coroutine<>::suspendThis([](auto h) {
        return std::noop_coroutine();
    });
    auto cid = serverRespState.requestHeader.method;
    ~co_await (this->*ServerHandler::Methods[cid])(serverRespState);
    lock.lock();
    finishedHandlers.emplace_back(serverRespState);
    lock.unlock();
}

async::Coroutine<>
ServerHandler::stubHandler(ServerResponseState &serverRespState) {
    BOOST_LOG_TRIVIAL(trace)
        << "stub handler#m" << (int) serverRespState.requestHeader.method
        << "(t" << serverRespState.requestHeader.tag << ") l"
        << serverRespState.requestHeader.requestLength;
    co_return;
}
}// namespace RingSwarm::proto
