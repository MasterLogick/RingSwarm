#include "PlainSocketTransport.h"
#include "TransportBackendException.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

namespace RingSwarm::transport {
    void PlainSocketTransport::rawWrite(void *data, uint32_t len) {
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
    }

    void PlainSocketTransport::close() {
        ::close(sockFd);
    }

    PlainSocketTransport::PlainSocketTransport(std::string &host, int port) {
        sockFd = socket(AF_INET, SOCK_STREAM, 0);
        sockaddr_in serv_addr{};
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(port);
        if (inet_pton(AF_INET, host.c_str(), &serv_addr.sin_addr) != 1) {
            throw TransportBackendException();
        }
        if (connect(sockFd, reinterpret_cast<const sockaddr *>(&serv_addr), sizeof(serv_addr)) != 0) {
            throw TransportBackendException();
        }
    }

    PlainSocketTransport::PlainSocketTransport(int sockFd) {
        this->sockFd = sockFd;
    }
}