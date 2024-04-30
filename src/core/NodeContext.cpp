#include "NodeContext.h"
#include "../crypto/SymmetricalCypher.h"
#include "../proto/ServerHandler.h"
#include "../transport/PlainSocketTransport.h"
#include "../transport/SecureOverlayTransport.h"
namespace RingSwarm::core {
int NodeContext::addServer(std::string host, int port) {
    auto &server = servers.emplace_back(std::make_unique<transport::PlainSocketServer>(std::move(host), port));
    return server->listen([this](std::unique_ptr<transport::PlainSocketTransport> serverSideSocket) {
        serverHandlers.push_back(handleServerConnection(std::move(serverSideSocket)));
    });
}
async::Coroutine<> NodeContext::handleServerConnection(std::unique_ptr<transport::PlainSocketTransport> serverSideSocket) {
    auto [a, b] = co_await transport::SecureOverlayTransport::createServerSide(std::move(serverSideSocket));
    core::Node remote(b);
    proto::ServerHandler serverHandler(std::move(a), remote);
    co_await serverHandler.listen();
}
}// namespace RingSwarm::core
