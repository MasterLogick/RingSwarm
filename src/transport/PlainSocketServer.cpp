#include <thread>
#include <boost/asio/ip/tcp.hpp>
#include "PlainSocketServer.h"
#include "PlainSocketTransport.h"
#include "../proto/ServerHandler.h"
#include <boost/log/trivial.hpp>
#include "connectionInfo/PlainSocketConnectionInfo.h"
#include "../async/EventLoop.h"
#include <uvw/dns.h>

namespace RingSwarm::transport {
    using namespace uvw;

    PlainSocketServer::PlainSocketServer(std::string &host, int port) :
            connectionInfo(new PlainSocketConnectionInfo(host, port)),
            serverHandler(async::getEventLoop()->resource<tcp_handle>()) {
        serverHandler->on<uvw::listen_event>([](auto &, auto &server) {
            auto socket = async::getEventLoop()->resource<uvw::tcp_handle>();
            server.accept(*socket);
//            async::getEventLoop()->stop();
            auto peer = socket->peer();
            BOOST_LOG_TRIVIAL(debug) << "Plain tcp server accepted connection from " << peer.ip << ":" << peer.port;
            proto::ServerHandler::Handle(new PlainSocketTransport(socket));
        });
        serverHandler->on<error_event>([](auto &evt, auto &) {
            BOOST_LOG_TRIVIAL(debug) << "Plain tcp server error: " << evt.what();
        });
        addrinfo hints;
        memset(&hints, 0, sizeof(struct addrinfo));
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_PASSIVE;
        auto req = async::getEventLoop()->resource<get_addr_info_req>();
        auto resp = req->addr_info_sync(host, std::to_string(port), &hints);
        if (resp.first) {
            for (auto x = resp.second.get(); x != nullptr; x = x->ai_next) {
                if (serverHandler->bind(*resp.second->ai_addr) == 0) {
                    return;
                }
            }
        }
        BOOST_LOG_TRIVIAL(trace) << "Plain tcp server could not listed on " << host << ":" << port;

    }

    void PlainSocketServer::listen() {
        BOOST_LOG_TRIVIAL(debug) << "Plain tcp server starts listening";
        serverHandler->listen();
    }

    ConnectionInfo *PlainSocketServer::getConnectionInfo() {
        return connectionInfo;
    }
}