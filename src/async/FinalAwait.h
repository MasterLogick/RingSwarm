#ifndef COROUTINES_FINALAWAIT_H
#define COROUTINES_FINALAWAIT_H

#include <glog/logging.h>

#include <coroutine>
#include <iostream>

namespace RingSwarm::async {
class FinalAwait {
    std::coroutine_handle<> caller;

public:
    FinalAwait(std::coroutine_handle<> caller) : caller(caller) {}

    bool await_ready() noexcept {
        return false;
    }

    std::coroutine_handle<> await_suspend(std::coroutine_handle<> h) {
        if (caller != nullptr) {
            return caller;
        } else {
            return std::noop_coroutine();
        }
    }

    void await_resume() noexcept {
        LOG(ERROR) << "===========PANIC===========";
        LOG(ERROR) << "Called unreachable function";
        LOG(ERROR) << "FinalAwait::await_resume();";
        LOG(ERROR) << "===========PANIC===========";
        std::abort();
    }
};
}// namespace RingSwarm::async

#endif// COROUTINES_FINALAWAIT_H
