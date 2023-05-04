#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <boost/asio/ip/tcp.hpp>
#include "PlainSocketServer.h"
#include "PlainSocketTransport.h"
#include "../proto/ServerHandler.h"
#include "TransportBackendException.h"
#include <boost/log/trivial.hpp>

namespace RingSwarm::transport {
    PlainSocketServer::PlainSocketServer(std::string &hostname, int port, int backlog) {
        sockFd = socket(AF_INET, SOCK_STREAM, 0);
        sockaddr_in serv_addr{};
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(port);
        bool d = true;
        if (setsockopt(sockFd, SOL_SOCKET, SO_REUSEADDR, &d, sizeof(int)) != 0) {
            throw TransportBackendException();
        }
        if (inet_pton(AF_INET, hostname.c_str(), &serv_addr.sin_addr) != 1) {
            throw TransportBackendException();
        }
        if (bind(sockFd, reinterpret_cast<const sockaddr *>(&serv_addr), sizeof(serv_addr)) != 0) {
            throw TransportBackendException();
        }
        if (::listen(sockFd, backlog) != 0) {
            throw TransportBackendException();
        }
    }

    void PlainSocketServer::listen() {
        BOOST_LOG_TRIVIAL(info) << "Plain tcp server starts listening";
        while (true) {
            sockaddr_in addr{};
            socklen_t size;
            int remoteFd = accept(sockFd, reinterpret_cast<sockaddr *>(&addr), &size);
            if (getpeername(remoteFd, reinterpret_cast<sockaddr *>(&addr), &size) != 0) {
                throw TransportBackendException();
            }
            BOOST_LOG_TRIVIAL(debug) << "Plain tcp server accepted connection from "
                                     << inet_ntoa(addr.sin_addr) << ":" << addr.sin_port;
            if (remoteFd == -1) {
                throw TransportBackendException();
            }
            auto *handler =
                    new proto::ServerHandler(new PlainSocketTransport(remoteFd));
            std::thread th([&handler] { handler->handleClientConnection(); });
            th.detach();
        }
    }
}