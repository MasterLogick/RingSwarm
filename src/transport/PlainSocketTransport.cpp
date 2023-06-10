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
                    async::interruptEventLoop();
                    return;
                }
            }
        }
        BOOST_LOG_TRIVIAL(trace) << "Plain tcp socket could not connect to " << host << ":" << port;
    }

    PlainSocketTransport::PlainSocketTransport(const std::shared_ptr<tcp_handle> &handle) : tcpHandler(handle) {
        setupHandler();
    }

    std::shared_ptr<async::Future<void>> PlainSocketTransport::rawRead(void *data, uint32_t size) {
        accessSpinlock.lock();
        if (currentRequest.data == nullptr) {
            currentRequest.data = static_cast<uint8_t *>(data);
            currentRequest.offset = 0;
            currentRequest.len = size;
            currentFuture = async::Future<void>::create();
            accessSpinlock.unlock();
            tcpHandler->read();
            async::interruptEventLoop();
            return currentFuture;
        } else {
            auto x = std::get<0>(readRequestQueue.emplace(async::Future<void>::create(), data, size));
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
        tcpHandler->set_allocator([this](auto *buf, auto, auto &) {
            accessSpinlock.lock();
            if (currentRequest.data != nullptr) {
                *buf = uv_buf_init(reinterpret_cast<char *>(currentRequest.data + currentRequest.offset),
                                   currentRequest.len - currentRequest.offset);
            } else {
                *buf = uv_buf_init(new char, 1);
            }
            accessSpinlock.unlock();
        });
        tcpHandler->on<data_event>([this](auto &event, auto &) {
            accessSpinlock.lock();
            if (currentRequest.data != nullptr) {
                currentRequest.offset += event.length;
                if (currentRequest.offset == currentRequest.len) {
                    BOOST_LOG_TRIVIAL(trace) << "Plain tcp sock |<=== "
                                             << boost::algorithm::hex(
                                                     std::string(reinterpret_cast<const char *>(currentRequest.data),
                                                                 currentRequest.len));
                    currentFuture->resolve();
                    if (!readRequestQueue.empty()) {
                        auto nextRequest = readRequestQueue.front();
                        readRequestQueue.pop();
                        currentRequest.data = static_cast<uint8_t *>(std::get<1>(nextRequest));
                        currentRequest.offset = 0;
                        currentRequest.len = std::get<2>(nextRequest);
                        currentFuture = std::get<0>(nextRequest);
                    } else {
                        currentRequest.data = nullptr;
                        tcpHandler->stop();
                    }
                }
                event.data.release();
            } else {
                if (event.length == UV_ENOBUFS) {
                    event.data.release();
                }
            }
            accessSpinlock.unlock();
        });
    }
}