#ifndef RINGSWARM_SRC_ASYNC_SUSPENDTHISCOROUTINEAWAITOBJECT_H
#define RINGSWARM_SRC_ASYNC_SUSPENDTHISCOROUTINEAWAITOBJECT_H

namespace RingSwarm::async {
template<typename... RetTypes>
class SuspendThisCoroutineAwaitObject {
    std::function<std::coroutine_handle<>(std::coroutine_handle<Promise<RetTypes...>>)> handler;

public:
    explicit SuspendThisCoroutineAwaitObject(std::function<std::coroutine_handle<>(std::coroutine_handle<Promise<RetTypes...>>)> handler) : handler(std::move(handler)) {}

    bool await_ready() noexcept {
        return false;
    }

    std::coroutine_handle<> await_suspend(std::coroutine_handle<Promise<RetTypes...>> h) {
        return handler(h);
    }

    void await_resume() noexcept {
    }
};

}// namespace RingSwatm::async


#endif//RINGSWARM_SRC_ASYNC_SUSPENDTHISCOROUTINEAWAITOBJECT_H
