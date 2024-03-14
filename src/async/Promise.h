#ifndef RINGSWARM_SRC_ASYNC_PROMISE_H
#define RINGSWARM_SRC_ASYNC_PROMISE_H

namespace RingSwarm::async {
template<class RetType>
class PromiseBase {
    void *nextCoro = nullptr;

public:
    template<class AwaitedRetType>
    ChainedAwaitObject<AwaitedRetType, RetType> await_transform(Coroutine<AwaitedRetType> h);

    std::suspend_never initial_suspend() noexcept {
        std::cout << "coroutine promise initial_suspend" << std::endl;
        return {};
    }

    FinalAwait final_suspend() noexcept {
        std::cout << "coroutine promise final_suspend" << std::endl;
        return {nextCoro};
    }

    void unhandled_exception() {
        std::cout << "coroutine promise unhandled_exception" << std::endl;
    }

    void setNextCoro(void *coro) {
        std::cout << "coroutine promise setNextCoro" << std::endl;
        nextCoro = coro;
    }
};

template<class RetType>
class Promise : public PromiseBase<RetType> {
    RetType val;

public:
    Coroutine<RetType> get_return_object();

    void return_value(RetType v) {
        std::cout << "coroutine promise return_value" << std::endl;
        val = std::move(v);
    }
};

template<>
class Promise<void> : public PromiseBase<void> {
    Coroutine<void> get_return_object();
};


}// namespace RingSwarm::async


#endif//RINGSWARM_SRC_ASYNC_PROMISE_H
