#include "NodeContext.h"

#include "../async/SyncCoroutineWaiter.h"
#include "../crypto/SymmetricalCypher.h"
#include "../proto/ServerHandler.h"
#include "../transport/PlainSocketTransport.h"
#include "../transport/SecureOverlayTransport.h"

namespace RingSwarm::core {
int NodeContext::addServer(std::string host, int port) {
    auto &server = servers.emplace_back(
        std::make_unique<transport::PlainSocketServer>(std::move(host), port)
    );
    return server->listen(
        [this](std::shared_ptr<transport::Transport> serverSideSocket) {
            std::lock_guard<std::mutex> _(serverHandlersMutex);
            serverHandlers.emplace_front();
            auto ref = serverHandlers.begin();
            ref->serverSideTransport = serverSideSocket;
            ref->coroutine =
                handleServerConnection(std::move(serverSideSocket), ref);
        }
    );
}

async::Coroutine<> NodeContext::handleServerConnection(
    std::shared_ptr<transport::Transport> serverSideSocket,
    std::list<ServerHandlerContext>::iterator ref
) {
    std::exception_ptr ptr;
    try {
        cleanupFinishedHandlers();
        auto [a, b] =
            co_await transport::SecureOverlayTransport::createServerSide(
                std::move(serverSideSocket)
            );
        core::Node remote(b);
        {
            proto::ServerHandler serverHandler(a, remote);
            ~co_await serverHandler.listen();
        }
    } catch (...) { ptr = std::current_exception(); }
    std::lock_guard<std::mutex> _s(serverHandlersMutex);
    {
        std::lock_guard<std::mutex> _f(finishedHandlersMutex);
        finishedHandlers.push_back(std::move(*ref));
    }
    serverHandlers.erase(ref);
    if (ptr) {
        std::rethrow_exception(ptr);
    }
}

NodeContext::~NodeContext() {
    {
        std::lock_guard<std::mutex> _(serverHandlersMutex);
        for (auto &item: serverHandlers) { item.serverSideTransport->close(); }
    }
    cleanupFinishedHandlers();
    async::SyncCoroutineWaiter<async::Coroutine<>> waiter;
    serverHandlersMutex.lock();
    while (!serverHandlers.empty()) {
        auto coroutine = std::move(serverHandlers.begin()->coroutine);
        serverHandlersMutex.unlock();
        waiter.wait(std::move(coroutine));
        serverHandlersMutex.lock();
    }
    serverHandlersMutex.unlock();
    cleanupFinishedHandlers();
}

void NodeContext::cleanupFinishedHandlers() {
    if (!finishedHandlers.empty()) {
        std::lock_guard<std::mutex> _(finishedHandlersMutex);
        for (const auto &item: finishedHandlers) {
            if (item.coroutine)
                item.coroutine.getHandle().promise().check();
        }
        finishedHandlers.clear();
    }
}
}// namespace RingSwarm::core
