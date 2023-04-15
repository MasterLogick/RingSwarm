#include <thread>
#include <iostream>
#include "transport/PlainSocketServer.h"
#include "transport/Transport.h"
#include "transport/PlainSocketTransport.h"
#include "proto/ClientHandler.h"
#include <coroutine>
#include <boost/asio.hpp>

int main() {
    boost::asio::io_context ioContext;
    std::string host("127.0.0.1");
    RingSwarm::transport::PlainSocketServer server(host, 12345, 8);
    std::thread serv([&server] { server.listen(); });
    RingSwarm::transport::PlainSocketTransport transport(host, 12345);
    RingSwarm::proto::ClientHandler client(&transport);
    RingSwarm::core::Id id{};
    client.unsubscribeOnChunkChange(id, 0);
    std::cout << "1" << std::endl;
    serv.join();
    boost::asio::io_context io;
}
//
//#include <boost/asio/co_spawn.hpp>
//#include <boost/asio/detached.hpp>
//#include <boost/asio/io_context.hpp>
//#include <boost/asio/ip/tcp.hpp>
//#include <boost/asio/signal_set.hpp>
//#include <boost/asio/write.hpp>
//#include <cstdio>
//
//using boost::asio::ip::tcp;
//using boost::asio::awaitable;
//using boost::asio::co_spawn;
//using boost::asio::detached;
//using boost::asio::use_awaitable;
//namespace this_coro = boost::asio::this_coro;
//
//awaitable<void> echo(tcp::socket socket) {
//    try {
//        char data[1024];
//        for (;;) {
//            std::size_t n = co_await socket.async_read_some(boost::asio::buffer(data), use_awaitable);
//            co_await async_write(socket, boost::asio::buffer(data, n), use_awaitable);
//        }
//    }
//    catch (std::exception &e) {
//        std::printf("echo Exception: %s\n", e.what());
//    }
//}
//
//awaitable<void> listener() {
//    auto executor = co_await this_coro::executor;
//    tcp::acceptor acceptor(executor, {tcp::v4(), 55555});
//    for (;;) {
//        tcp::socket socket = co_await acceptor.async_accept(use_awaitable);
//        co_spawn(executor, echo(std::move(socket)), detached);
//    }
//}
//
//int main() {
//    try {
//        boost::asio::io_context io_context;
//
//        boost::asio::signal_set signals(io_context, SIGINT, SIGTERM);
//        signals.async_wait([&](auto, auto) { io_context.stop(); });
//
//        co_spawn(io_context, listener(), detached);
//
//        io_context.run();
//    }
//    catch (std::exception &e) {
//        std::printf("Exception: %s\n", e.what());
//    }
//}