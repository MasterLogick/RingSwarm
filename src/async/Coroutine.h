#ifndef COROUTINES_COROUTINE_H
#define COROUTINES_COROUTINE_H

#include "FinalAwait.h"
#include <coroutine>
#include <iostream>

namespace RingSwarm::async {
template<class RetType>
class Coroutine {
    std::coroutine_handle<Promise<RetType>> handle;

public:
    explicit Coroutine(std::coroutine_handle<Promise<RetType>> handle) : handle(std::move(handle)) {
        std::cout << "coroutine common constructor" << std::endl;
    }

    Coroutine(const Coroutine<RetType> &c) = delete;

    Coroutine<RetType> &operator=(const Coroutine<RetType> &) = delete;

    Coroutine(Coroutine<RetType> &&c) noexcept {
        handle = c.handle;
        c.handle = nullptr;
    }

    ~Coroutine() {
        if (handle) {
            handle.destroy();
        }
        std::cout << "coroutine delete" << std::endl;
    }

    const std::coroutine_handle<Promise<RetType>> &getHandle() const {
        return handle;
    }
};
}// namespace RingSwarm::async

#endif//COROUTINES_COROUTINE_H
