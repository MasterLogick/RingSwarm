#include "async/EventLoop.h"
#include "async/SyncCoroutineWaiter.h"
#include "async/ThreadPool.h"
#include "core/NodeContext.h"
#include "proto/ClientTransport.h"
#include "transport/PlainSocketTransport.h"

#include <glog/logging.h>

using namespace RingSwarm;

async::Coroutine<> test() {
    constexpr int n = 4096;
    std::shared_ptr<proto::ClientTransport> client[n];
    for (int i = 0; i < n; ++i) {
        auto tr = transport::PlainSocketTransport::Create();
        int conn = co_await tr->connect("localhost", 12345);
        if (conn != 0) {
            co_return;
        }
        client[i] = std::make_shared<proto::ClientTransport>(std::move(tr));
    }

    async::Coroutine<transport::Buffer> coros[n * 2];
    for (uint16_t i = 0; i < n; ++i) {
        proto::RequestBuffer req(0);
        coros[i * 2] = client[i]->sendSmallRequest(
            RingSwarm::proto::CommandId_Ping,
            req,
            0
        );
        coros[i * 2 + 1] = client[i]->sendSmallRequest(
            RingSwarm::proto::CommandId_Ping,
            req,
            0
        );
    }
    for (const auto &item: coros) {
        ~co_await (async::Coroutine<transport::Buffer> &&) std::move(item);
    }
    VLOG(1) << "send data";
}

int main(int argc, char **argv, char **envp) {
    FLAGS_logtostdout = true;
    FLAGS_v = 10;
    google::InitGoogleLogging("RingSwarm");

    async::ThreadPool::setDefaultThreadPool(new async::ThreadPool(16));
    async::EventLoop::setMainEventLoop(new async::EventLoop());
    auto mainEventLoopRunRes = async::EventLoop::getMainEventLoop()->run();
    if (!mainEventLoopRunRes) {
        VLOG(1) << "Main event loop initialization failed";
        return -1;
    }

    {
        core::NodeContext ctx;
        ctx.addServer("0.0.0.0", 12345);
        async::Coroutine<> c = test();
        async::SyncCoroutineWaiter<async::Coroutine<>> waiter;
        waiter.wait(std::move(c));
    }

    async::ThreadPool::getDefaultThreadPool()->waitEmpty();
    async::EventLoop::getMainEventLoop()->stop();
    async::ThreadPool::getDefaultThreadPool()->blockingStop();
    VLOG(1) << "end";
}
