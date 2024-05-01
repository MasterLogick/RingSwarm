#ifndef COROUTINES_COROUTINE_H
#define COROUTINES_COROUTINE_H

#include "FinalAwait.h"
#include "ThreadPool.h"

#include <coroutine>
#include <functional>
#include <iostream>

namespace RingSwarm::async {
template<class... RetTypes>
class Coroutine {
    std::coroutine_handle<Promise<RetTypes...>> handle;

public:
    static GetCoroutineHandleAwaitObject<RetTypes...> getThisCoroutineHandle() {
        return GetCoroutineHandleAwaitObject<RetTypes...>();
    }

    static SuspendThisCoroutineAwaitObject<RetTypes...> suspendThis(auto f) {
        return SuspendThisCoroutineAwaitObject<RetTypes...>(std::move(f));
    }

    static void
    scheduleCoroutineResume(std::coroutine_handle<Promise<RetTypes...>> f) {
        ThreadPool::getDefaultThreadPool()->resumeCoroutine(f.address());
    }

    Coroutine() = default;

    explicit Coroutine(std::coroutine_handle<Promise<RetTypes...>> handle)
        : handle(std::move(handle)) {}

    Coroutine(const Coroutine<RetTypes...> &c) = delete;

    Coroutine<RetTypes...> &operator=(const Coroutine<RetTypes...> &) = delete;

    Coroutine<RetTypes...> &operator=(Coroutine<RetTypes...> &&src) noexcept {
        if (handle != nullptr) {
            handle.destroy();
        }
        handle = src.handle;
        src.handle = nullptr;
        return *this;
    }

    Coroutine(Coroutine<RetTypes...> &&c) noexcept {
        handle = c.handle;
        c.handle = nullptr;
    }

    ~Coroutine() {
        if (handle != nullptr) {
            handle.destroy();
        }
    }

    const std::coroutine_handle<Promise<RetTypes...>> &getHandle() const {
        return handle;
    }
};
}// namespace RingSwarm::async

#endif// COROUTINES_COROUTINE_H
