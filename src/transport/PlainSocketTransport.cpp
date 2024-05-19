#include "PlainSocketTransport.h"

#include "../Assert.h"
#include "../async/EventLoop.h"

#include "ClosedTransportException.h"

#include <glog/logging.h>
#include <uvw/dns.h>

#include <memory>

namespace RingSwarm::transport {

PlainSocketTransport::PlainSocketTransport(
    const std::shared_ptr<uvw::tcp_handle> &handle,
    Private
)
    : tcpHandler(handle) {
    bindAddress = handle->sock();
    peerAddress = handle->peer();
}

std::shared_ptr<PlainSocketTransport> PlainSocketTransport::Create() {
    auto ell = async::EventLoop::getMainEventLoop()->acquireEventLoopLock();
    auto tcpHandler = async::EventLoop::getMainEventLoop()->createTcpSocket();
    auto ptr = std::make_shared<PlainSocketTransport>(
        std::move(tcpHandler),
        Private()
    );
    ptr->setupHandler();
    return std::move(ptr);
}

std::shared_ptr<PlainSocketTransport>
PlainSocketTransport::Create(const std::shared_ptr<uvw::tcp_handle> &handle) {
    auto ptr = std::make_shared<PlainSocketTransport>(handle, Private());
    auto ell = async::EventLoop::getMainEventLoop()->acquireEventLoopLock();
    ptr->setupHandler();
    return std::move(ptr);
}

async::Coroutine<int>
PlainSocketTransport::connect(std::string host, int port) {
    std::coroutine_handle<async::Promise<int>> handle =
        co_await async::Coroutine<int>::getThisCoroutineHandle();
    int err;
    auto req = async::EventLoop::getMainEventLoop()->createAddrInfoReq();

    auto [success, resp] = req->addr_info_sync(host, std::to_string(port));
    if (!success) {
        VLOG(1) << "Could not resolve address " << host << ":" << port;
        co_return -1;
    }
    {
        auto ell = async::EventLoop::getMainEventLoop()->acquireEventLoopLock();
        tcpHandler->on<uvw::connect_event>([&handle, this](auto a, auto &b) {
            peerAddress = b.peer();
            bindAddress = b.sock();
            VLOG(3) << "Plain tcp socket connected to " << peerAddress.ip << ":"
                    << peerAddress.port << " from " << bindAddress.ip << ":"
                    << bindAddress.port;
            async::Coroutine<int>::scheduleCoroutineResume(handle);
        });
        tcpHandler->on<uvw::error_event>([&handle, &err](auto a, auto &b) {
            err = a.code();
            async::Coroutine<int>::scheduleCoroutineResume(handle);
        });
        err = tcpHandler->connect(*resp->ai_addr);
        if (err != 0) {
            VLOG(3) << "Plain tcp socket could not connect to " << host << ":"
                    << port;
        }
    }
    co_await async::Coroutine<int>::suspendThis(
        [](std::coroutine_handle<async::Promise<int>> h) {
            return std::noop_coroutine();
        }
    );
    co_return err;
}

async::Coroutine<> PlainSocketTransport::rawRead(void *data, uint32_t size) {
#ifndef NDEBUG
    Assert(!readFlag.test_and_set(), "simultaneous read on tcp socket");
#endif
    if (size == 0) {
#ifndef NDEBUG
        readFlag.clear();
#endif
        co_return;
    }
    readBufferData = static_cast<char *>(data);
    readBufferOffset = 0;
    readBufferSize = size;
    co_await async::Coroutine<>::suspendThis(
        [this](std::coroutine_handle<async::Promise<>> h) {
            auto ell =
                async::EventLoop::getMainEventLoop()->acquireEventLoopLock();
            if (tcpHandler->closing()) {
                raiseClosedTransportException(h);
                return static_cast<std::coroutine_handle<>>(h);
            }
            readPromiseHandle = h;
            tcpHandler
                ->read<[](const uvw::tcp_handle &handler, auto preferredSize) {
                    auto pst =
                        handler.data<std::weak_ptr<PlainSocketTransport>>();
                    if (auto sp = pst->lock()) {
                        return std::make_pair(
                            sp->readBufferData + sp->readBufferOffset,
                            sp->readBufferSize - sp->readBufferOffset
                        );
                    } else {
                        return std::make_pair<char *, unsigned int>(nullptr, 0);
                    }
                }>();
            return static_cast<std::coroutine_handle<>>(std::noop_coroutine());
        }
    );
}

void PlainSocketTransport::raiseClosedTransportException(
    std::coroutine_handle<async::Promise<>> h
) {
    readBufferOffset = 0;
    readBufferSize = 0;
    readBufferData = nullptr;
    readPromiseHandle = nullptr;
#ifndef NDEBUG
    Assert(readFlag.test(), "read flag must be raised");
    readFlag.clear();
#endif
    h.promise().raiseException(ClosedTransportException());
}

void PlainSocketTransport::rawWrite(void *data, uint32_t len) {
#ifndef NDEBUG
    Assert(!writeFlag.test_and_set(), "simultaneous write to tcp socket");
#endif
    while (tcpHandler->try_write(static_cast<char *>(data), len) == UV_EAGAIN) {
    }
#ifndef NDEBUG
    writeFlag.clear();
#endif
}

void PlainSocketTransport::close() {
    auto ell = async::EventLoop::getMainEventLoop()->acquireEventLoopLock();
    tcpHandler->close();
}

PlainSocketTransport::~PlainSocketTransport() {
    auto ell = async::EventLoop::getMainEventLoop()->acquireEventLoopLock();
    onClose();
}

void PlainSocketTransport::setupHandler() {
    auto th = weak_from_this();
    tcpHandler->data(
        std::make_shared<std::weak_ptr<PlainSocketTransport>>(weak_from_this())
    );
    tcpHandler->on<uvw::end_event>([th](auto &, auto &handler) {
        if (auto sp = th.lock()) {
            VLOG(3) << "Plain tcp socket closed connection from "
                    << sp->peerAddress.ip << ":" << sp->peerAddress.port;
            auto h = sp->readPromiseHandle;
            if (h) {
                sp->raiseClosedTransportException(h);
                async::Coroutine<>::scheduleCoroutineResume(h);
            }
        }
    });
    tcpHandler->on<uvw::close_event>([th](auto &ev, auto &handler) {
        if (auto sp = th.lock()) {
            VLOG(3) << "Plain tcp socket closed connection to "
                    << sp->peerAddress.ip << ":" << sp->peerAddress.port;
            sp->onClose();
        }
    });
    tcpHandler->on<uvw::error_event>([th](auto &evt, auto &handler) {
        if (auto sp = th.lock()) {
            VLOG(1) << "Plain tcp socket " << sp->peerAddress.ip << ":"
                    << sp->peerAddress.port << " error " << evt.name();
            VLOG(1) << evt.what();
            sp->onClose();
        }
    });
    tcpHandler->on<uvw::data_event>([th, this](auto &event, auto &handler) {
        Assert(!th.expired(), "transport must be still alive on read event");
        event.data.release();
        if (event.length == UV_ENOBUFS) {
            handler.stop();
            return;
        } else if (((ssize_t) event.length) > 0) {
            Assert(
                readPromiseHandle,
                "must have pending request to process data event"
            );
            readBufferOffset += event.length;
            if (readBufferOffset == readBufferSize) {
                readBufferData = nullptr;
                readBufferOffset = 0;
                readBufferSize = 0;
                Assert(
                    readPromiseHandle,
                    "read promise handle must be set on data read finish"
                );
                auto handle = readPromiseHandle;
                readPromiseHandle = nullptr;
                tcpHandler->stop();
#ifndef NDEBUG
                Assert(readFlag.test(), "read flag must be raised");
                readFlag.clear();
#endif
                async::Coroutine<>::scheduleCoroutineResume(handle);
            }
        }
    });
}

void PlainSocketTransport::onClose() {
    if (readPromiseHandle) {
        auto h = readPromiseHandle;
        raiseClosedTransportException(readPromiseHandle);
        async::Coroutine<>::scheduleCoroutineResume(h);
    }
    tcpHandler->close();
}
}// namespace RingSwarm::transport
