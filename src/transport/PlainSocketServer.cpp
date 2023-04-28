#include <boost/asio/use_awaitable.hpp>
#include <boost/asio/co_spawn.hpp>
#include <boost/asio/detached.hpp>
#include "BoostSocketServer.h"
#include "../proto/ServerHandler.h"
#include "TransportBackendException.h"

namespace RingSwarm::transport {
    using namespace boost::asio;

    BoostSocketServer::BoostSocketServer(int port) {
        this->port = port;
    }

    awaitable<void> BoostSocketServer::listen() {
        auto executor = co_await boost::asio::this_coro::executor;
        boost::asio::ip::tcp::acceptor acceptor(executor);
        acceptor.open(boost::asio::ip::tcp::v4());
        acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
        boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::tcp::v4(), port);
        acceptor.bind(endpoint);
        while (true) {
            ip::tcp::socket socket = co_await acceptor.async_accept(use_awaitable);
            co_spawn(executor, [&socket]() -> awaitable<void> {
                         proto::ServerHandler handler(std::move(socket));
                         co_await handler.handleClientConnection();
                     },
                     detached);
        }
    }
}