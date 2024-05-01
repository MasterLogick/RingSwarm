#ifndef RINGSWARM_SRC_ASYNC_GETCOROUTINEHANDLEAWAITOBJECT_H
#define RINGSWARM_SRC_ASYNC_GETCOROUTINEHANDLEAWAITOBJECT_H

#include <coroutine>
#include <functional>

namespace RingSwarm::async {
template<typename... RetTypes>
class GetCoroutineHandleAwaitObject {
    std::coroutine_handle<Promise<RetTypes...>> handle;

public:
    bool await_ready() noexcept {
        return false;
    }

    std::coroutine_handle<>
    await_suspend(std::coroutine_handle<Promise<RetTypes...>> h) {
        this->handle = h;
        return h;
    }

    std::coroutine_handle<Promise<RetTypes...>> await_resume() noexcept {
        return handle;
    }
};
}// namespace RingSwarm::async

#endif// RINGSWARM_SRC_ASYNC_GETCOROUTINEHANDLEAWAITOBJECT_H
