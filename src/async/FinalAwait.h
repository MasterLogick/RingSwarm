#ifndef COROUTINES_FINALAWAIT_H
#define COROUTINES_FINALAWAIT_H

#include <coroutine>
#include <iostream>

namespace RingSwarm::async {
class FinalAwait {
public:
    FinalAwait(void *nextCoro) : nextCoro(nextCoro) {
        std::cout << "final await constructor" << std::endl;
    }

    bool await_ready() noexcept {
        std::cout << "final await await_ready" << std::endl;
        return false;
    }

    std::coroutine_handle<> await_suspend(std::coroutine_handle<> h) {
        std::cout << "final await await_suspend" << std::endl;
        if (nextCoro != nullptr) {
            return std::coroutine_handle<>::from_address(nextCoro);
        } else {
            return std::noop_coroutine();
        }
    }

    void await_resume() noexcept {
        std::cerr << "===========PANIC===========" << std::endl
                  << "Called unreachable function" << std::endl
                  << "FinalAwait::await_resume();" << std::endl
                  << "===========PANIC===========" << std::endl;
        std::abort();
    }

    void *nextCoro;
};
}// namespace RingSwarm::async

#endif//COROUTINES_FINALAWAIT_H
