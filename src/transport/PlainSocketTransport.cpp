#include "PlainSocketTransport.h"
#include "TransportBackendException.h"
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <string>
#include <boost/log/trivial.hpp>
#include <boost/algorithm/hex.hpp>

namespace RingSwarm::transport {
    void PlainSocketTransport::rawWrite(void *data, uint32_t len) {
        BOOST_LOG_TRIVIAL(trace) << "Plain tcp sock |===> "
                                 << boost::algorithm::hex(std::string(static_cast<char *>(data), len));
        for (uint32_t i = 0; i < len;) {
            auto sent = send(sockFd, reinterpret_cast<uint8_t *>(data) + i, len, 0);
            if (sent == -1) {
                throw TransportBackendException();
            }
            i += sent;
        }
    }

    void PlainSocketTransport::rawRead(void *buff, uint32_t len) {
        for (uint32_t i = 0; i < len;) {
            auto sent = recv(sockFd, reinterpret_cast<uint8_t *>(buff) + i, len, 0);
            if (sent == -1) {
                std::cout << strerror(errno) << std::endl;
                throw TransportBackendException();
            }
            i += sent;
        }
        BOOST_LOG_TRIVIAL(trace) << "Plain tcp sock |<=== "
                                 << boost::algorithm::hex(std::string(static_cast<char *>(buff), len));
    }

    void PlainSocketTransport::close() {
        ::close(sockFd);
    }

    PlainSocketTransport::PlainSocketTransport(std::string &host, int port) {
        addrinfo hints{};
        addrinfo *res, *rp;
        memset(&hints, 0, sizeof(struct addrinfo));
        hints.ai_family = AF_UNSPEC;
        hints.ai_socktype = SOCK_STREAM;
        std::string portStr = std::to_string(port);
        if (getaddrinfo(host.c_str(), portStr.c_str(), &hints, &res) != 0) {
            throw TransportBackendException();
        }
        for (rp = res; rp != nullptr; rp = rp->ai_next) {
            sockFd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
            if (sockFd == -1) {
                continue;
            }
            if (connect(sockFd, rp->ai_addr, rp->ai_addrlen) != -1) {
                break;
            }
            ::close(sockFd);
        }
        if (rp == nullptr) {
            throw TransportBackendException();
        }
        freeaddrinfo(res);
    }

    PlainSocketTransport::PlainSocketTransport(int sockFd) {
        this->sockFd = sockFd;
    }
}