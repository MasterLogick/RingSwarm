#ifndef RINGSWARM_SRC_ASYNC_PROMISE_H
#define RINGSWARM_SRC_ASYNC_PROMISE_H

namespace RingSwarm::async {
template<class... RetTypes>
class PromiseBase {
    void *nextCoro = nullptr;

public:
    template<class... AwaitedRetTypes>
    ChainedAwaitObject<Coroutine<AwaitedRetTypes...>, RetTypes...> await_transform(Coroutine<AwaitedRetTypes...> h) {
        std::cout << "coroutine promise await transform" << std::endl;
        return ChainedAwaitObject<Coroutine<AwaitedRetTypes...>, RetTypes...>(std::move(h));
    }

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

template<class... RetTypes>
class Promise : public PromiseBase<RetTypes...> {
    std::tuple<RetTypes...> val;

public:
    Coroutine<RetTypes...> get_return_object() {
        std::cout << "coroutine promise get_return_object" << std::endl;
        return Coroutine<RetTypes...>(std::coroutine_handle<Promise<RetTypes...>>::from_promise(*this));
    }

    void return_value(RetTypes... v) {
        std::cout << "coroutine promise return_value" << std::endl;
        val = {std::move(v)...};
    }
};

template<class RetType>
class Promise<RetType> : public PromiseBase<RetType> {
    RetType val;

public:
    Coroutine<RetType> get_return_object() {
        std::cout << "coroutine promise get_return_object" << std::endl;
        return Coroutine<RetType>(std::coroutine_handle<Promise<RetType>>::from_promise(*this));
    }

    void return_value(RetType v) {
        std::cout << "coroutine promise return_value" << std::endl;
        val = std::move(v);
    }
};

template<>
class Promise<> : public PromiseBase<> {
public:
    Coroutine<> get_return_object() {
        std::cout << "coroutine promise get_return_object" << std::endl;
        return Coroutine<>(std::coroutine_handle<Promise<>>::from_promise(*this));
    }

    void return_void() {
        std::cout << "coroutine promise return_value" << std::endl;
    }
};


}// namespace RingSwarm::async


#endif//RINGSWARM_SRC_ASYNC_PROMISE_H
