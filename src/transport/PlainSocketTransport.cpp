#include "PlainSocketTransport.h"
#include "../async/EventLoop.h"
#include <boost/log/trivial.hpp>
#include <boost/algorithm/hex.hpp>
#include <uvw/dns.h>

namespace RingSwarm::transport {
    using namespace uvw;

    PlainSocketTransport::PlainSocketTransport(std::string &host, int port) {
        auto req = async::getEventLoop()->resource<get_addr_info_req>();
        addrinfo hints;
        memset(&hints, 0, sizeof(struct addrinfo));
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        auto resp = req->addr_info_sync(host, std::to_string(port));
        if (!resp.first) {
            //todo throw exception
        }
        tcpHandler = async::getEventLoop()->resource<tcp_handle>();
        setupHandler();
        for (auto x = resp.second.get(); x != nullptr; x = x->ai_next) {
            if (x->ai_addr->sa_family == AF_INET) {
                if (tcpHandler->connect(*x->ai_addr) == 0) {
                    return;
                }
            }
        }
        BOOST_LOG_TRIVIAL(trace) << "Plain tcp socket could not connect to " << host << ":" << port;
    }

    PlainSocketTransport::PlainSocketTransport(const std::shared_ptr<tcp_handle> &handle) : tcpHandler(handle) {
        setupHandler();
    }

    std::shared_ptr<async::Future<uint8_t *>> PlainSocketTransport::rawRead(uint32_t size) {
        accessSpinlock.lock();
        if (currentRequest.data == nullptr) {
            currentRequest.data = new uint8_t[size];
            currentRequest.offset = 0;
            currentRequest.len = size;
            currentFuture = async::Future<uint8_t *>::create();
            accessSpinlock.unlock();
            tcpHandler->read();
            return currentFuture;
        } else {
            auto x = readRequestQueue.emplace(async::Future<uint8_t *>::create(), size).first;
            accessSpinlock.unlock();
            return x;
        }
    }

    void PlainSocketTransport::rawWrite(void *data, uint32_t len) {
        BOOST_LOG_TRIVIAL(trace) << "Plain tcp sock |===> "
                                 << boost::algorithm::hex(std::string(static_cast<char *>(data), len));
        tcpHandler->write(static_cast<char *>(data), len);
    }

    void PlainSocketTransport::close() {
        tcpHandler->close();
    }

    PlainSocketTransport::~PlainSocketTransport() {
        close();
    }

    void PlainSocketTransport::setupHandler() {
        tcpHandler->on<connect_event>([](auto &, auto &handler) {
            auto peer = handler.peer();
            BOOST_LOG_TRIVIAL(trace) << "Plain tcp socket connected to " << peer.ip << ":" << peer.port;
        });
        tcpHandler->on<end_event>([](auto &, auto &handler) {
            auto peer = handler.peer();
            BOOST_LOG_TRIVIAL(trace) << "Plain tcp socket ended connection to " << peer.ip << ":" << peer.port;
        });
        tcpHandler->on<close_event>([](auto &, auto &handler) {
            auto peer = handler.peer();
            BOOST_LOG_TRIVIAL(trace) << "Plain tcp socket closed connection to " << peer.ip << ":" << peer.port;
        });
        tcpHandler->on<error_event>([](auto &evt, auto &handler) {
            auto peer = handler.peer();
            BOOST_LOG_TRIVIAL(trace) << "Plain tcp socket " << peer.ip << ":" << peer.port << " error " << evt.name();
        });
        tcpHandler->on<data_event>([this](auto &event, auto &) {
            auto *data = reinterpret_cast<uint8_t *>(event.data.release());
            uint32_t offset = 0;
            while (true) {
                uint32_t copySize = std::min<uint32_t>(event.length, currentRequest.len - currentRequest.offset);
                memcpy(currentRequest.data, data, copySize);
                currentRequest.offset += copySize;
                offset += copySize;
                if (currentRequest.offset == currentRequest.len) {
                    BOOST_LOG_TRIVIAL(trace) << "Plain tcp sock |<=== "
                                             << boost::algorithm::hex(
                                                     std::string(reinterpret_cast<const char *>(currentRequest.data),
                                                                 currentRequest.len));
                    currentFuture->resolve(currentRequest.data);
                    accessSpinlock.lock();
                    if (!readRequestQueue.empty()) {
                        auto nextRequest = readRequestQueue.front();
                        readRequestQueue.pop();
                        accessSpinlock.unlock();
                        currentRequest.data = new uint8_t[nextRequest.second];
                        currentRequest.offset = 0;
                        currentRequest.len = nextRequest.second;
                        currentFuture = nextRequest.first;
                    } else {
                        accessSpinlock.unlock();
                        tcpHandler->stop();
                        if (offset != event.length) {
                            pending.data = data;
                            pending.offset = offset;
                            pending.len = event.length;
                        }
                        return;
                    }
                } else {
                    //data buffer is empty
                    delete data;
                    return;
                }
            }
        });
    }
}