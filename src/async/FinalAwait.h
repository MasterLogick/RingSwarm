#ifndef COROUTINES_FINALAWAIT_H
#define COROUTINES_FINALAWAIT_H

#include <coroutine>
#include <iostream>

#include <boost/log/trivial.hpp>

namespace RingSwarm::async {
class FinalAwait {
    void *nextCoro;

public:
    FinalAwait(void *nextCoro) : nextCoro(nextCoro) {
    }

    bool await_ready() noexcept {
        return false;
    }

    std::coroutine_handle<> await_suspend(std::coroutine_handle<> h) {
        if (nextCoro != nullptr) {
            return std::coroutine_handle<>::from_address(nextCoro);
        } else {
            return std::noop_coroutine();
        }
    }

    void await_resume() noexcept {
        BOOST_LOG_TRIVIAL(error) << "===========PANIC===========";
        BOOST_LOG_TRIVIAL(error) << "Called unreachable function";
        BOOST_LOG_TRIVIAL(error) << "FinalAwait::await_resume();";
        BOOST_LOG_TRIVIAL(error) << "===========PANIC===========";
        std::abort();
    }
};
}// namespace RingSwarm::async

#endif//COROUTINES_FINALAWAIT_H
