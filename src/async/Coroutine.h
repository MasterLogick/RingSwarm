#ifndef COROUTINES_COROUTINE_H
#define COROUTINES_COROUTINE_H

#include "FinalAwait.h"
#include <coroutine>
#include <iostream>

namespace RingSwarm::async {
template<class... RetTypes>
class Coroutine {
    std::coroutine_handle<Promise<RetTypes...>> handle;

public:
    explicit Coroutine(std::coroutine_handle<Promise<RetTypes...>> handle) : handle(std::move(handle)) {
        std::cout << "coroutine common constructor" << std::endl;
    }

    Coroutine(const Coroutine<RetTypes...> &c) = delete;

    Coroutine<RetTypes...> &operator=(const Coroutine<RetTypes...> &) = delete;

    Coroutine(Coroutine<RetTypes...> &&c) noexcept {
        handle = c.handle;
        c.handle = nullptr;
    }

    ~Coroutine() {
        if (handle) {
            handle.destroy();
        }
        std::cout << "coroutine delete" << std::endl;
    }

    const std::coroutine_handle<Promise<RetTypes...>> &getHandle() const {
        return handle;
    }
};
}// namespace RingSwarm::async

#endif//COROUTINES_COROUTINE_H
