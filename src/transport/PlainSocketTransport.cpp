#include "PlainSocketTransport.h"
#include "../Assert.h"
#include "../async/EventLoop.h"
#include <boost/algorithm/hex.hpp>
#include <boost/log/trivial.hpp>
#include <memory>
#include <uvw/dns.h>

namespace RingSwarm::transport {

PlainSocketTransport::PlainSocketTransport() {
    auto ell = async::EventLoop::getMainEventLoop()->acquireEventLoopLock();
    tcpHandler = async::EventLoop::getMainEventLoop()->createTcpSocket();
    setupHandler();
}

PlainSocketTransport::PlainSocketTransport(const std::shared_ptr<uvw::tcp_handle> &handle) : tcpHandler(handle) {
    bindAddress = handle->sock();
    peerAddress = handle->peer();
    auto ell = async::EventLoop::getMainEventLoop()->acquireEventLoopLock();
    setupHandler();
}

async::Coroutine<int> PlainSocketTransport::connect(std::string host, int port) {
    std::coroutine_handle<async::Promise<int>> handle = co_await async::Coroutine<int>::getThisCoroutineHandle();
    int err;
    auto req = async::EventLoop::getMainEventLoop()->createAddrInfoReq();

    auto [success, resp] = req->addr_info_sync(host, std::to_string(port));
    if (!success) {
        BOOST_LOG_TRIVIAL(trace) << "Could not resolve address " << host << ":" << port;
        co_return -1;
    }
    {
        auto ell = async::EventLoop::getMainEventLoop()->acquireEventLoopLock();
        tcpHandler->on<uvw::connect_event>([&handle, this](auto a, auto &b) {
            peerAddress = b.peer();
            bindAddress = b.sock();
            BOOST_LOG_TRIVIAL(debug) << "Plain tcp socket connected to " << peerAddress.ip << ":" << peerAddress.port << " from " << bindAddress.ip << ":" << bindAddress.port;
            async::Coroutine<int>::scheduleCoroutineResume(handle);
        });
        tcpHandler->on<uvw::error_event>([&handle, &err](auto a, auto &b) {
            err = a.code();
            async::Coroutine<int>::scheduleCoroutineResume(handle);
        });
        err = tcpHandler->connect(*resp->ai_addr);
        if (err != 0) {
            BOOST_LOG_TRIVIAL(trace) << "Plain tcp socket could not connect to " << host << ":" << port;
        }
    }
    co_await async::Coroutine<int>::suspendThis([](std::coroutine_handle<async::Promise<int>> h) {
        return std::noop_coroutine();
    });
    co_return err;
}

async::Coroutine<> PlainSocketTransport::rawRead(void *data, uint32_t size) {
#ifndef NDEBUG
    Assert(!readFlag.test_and_set(), "simultaneous read on tcp socket");
#endif
    if (size == 0) {
        readFlag.clear();
        co_return;
    }
    readBufferData = static_cast<char *>(data);
    readBufferOffset = 0;
    readBufferSize = size;
    co_await async::Coroutine<>::suspendThis([this](std::coroutine_handle<async::Promise<>> h) {
        readPromiseHandle = h;
        auto ell = async::EventLoop::getMainEventLoop()->acquireEventLoopLock();
        tcpHandler->read<[](const uvw::tcp_handle &handler, auto preferredSize) {
            auto *pst = *static_cast<PlainSocketTransport **>(handler.data().get());
            return std::make_pair(pst->readBufferData + pst->readBufferOffset, pst->readBufferSize - pst->readBufferOffset);
        }>();
        return std::noop_coroutine();
    });
}

void PlainSocketTransport::rawWrite(void *data, uint32_t len) {
#ifndef NDEBUG
    Assert(writeMutex.try_lock(), "simultaneous write to tcp socket");
#endif
    /*BOOST_LOG_TRIVIAL(trace) << "Plain tcp sock |===> "
                                     << boost::algorithm::hex(std::string(static_cast<char *>(data), len));*/
    while (tcpHandler->try_write(static_cast<char *>(data), len) == UV_EAGAIN) {}
#ifndef NDEBUG
    writeMutex.unlock();
#endif
}

void PlainSocketTransport::close() {
    auto ell = async::EventLoop::getMainEventLoop()->acquireEventLoopLock();
    tcpHandler->close();
}

PlainSocketTransport::~PlainSocketTransport() {
    auto ell = async::EventLoop::getMainEventLoop()->acquireEventLoopLock();
    tcpHandler->close();
}

void PlainSocketTransport::setupHandler() {
    tcpHandler->data(std::make_shared<PlainSocketTransport *>(this));
    tcpHandler->on<uvw::end_event>([this](auto &, auto &handler) {
        BOOST_LOG_TRIVIAL(trace) << "Plain tcp socket ended connection to " << peerAddress.ip << ":" << peerAddress.port;
    });
    tcpHandler->on<uvw::close_event>([this](auto &ev, auto &handler) {
        BOOST_LOG_TRIVIAL(trace) << "Plain tcp socket closed connection to " << peerAddress.ip << ":" << peerAddress.port;
    });
    tcpHandler->on<uvw::error_event>([this](auto &evt, auto &handler) {
        BOOST_LOG_TRIVIAL(trace) << "Plain tcp socket " << peerAddress.ip << ":" << peerAddress.port << " error " << evt.name();
    });
    tcpHandler->on<uvw::data_event>([this](auto &event, auto &handler) {
        if (event.length == UV_ENOBUFS) {
            event.data.release();
            handler.stop();
            return;
        } else {
            readBufferOffset += event.length;
            event.data.release();
            if (readBufferOffset == readBufferSize) {
                readBufferData = nullptr;
                readBufferOffset = 0;
                readBufferSize = 0;
#ifndef NDEBUG
                Assert(readFlag.test(), "read flag must be raised");
                readFlag.clear();
#endif
                Assert(readPromiseHandle, "read promise handle must be set on data read finish");
                auto handle = readPromiseHandle;
                readPromiseHandle = nullptr;
                tcpHandler->stop();
                async::Coroutine<>::scheduleCoroutineResume(handle);
            }
        }
    });
}
}// namespace RingSwarm::transport