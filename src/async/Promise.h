#ifndef RINGSWARM_SRC_ASYNC_PROMISE_H
#define RINGSWARM_SRC_ASYNC_PROMISE_H

namespace RingSwarm::async {
template<class... RetTypes>
class PromiseBase {
    void *nextCoro = nullptr;

public:
    template<class... AwaitedRetTypes>
    ChainedAwaitObject<Coroutine<AwaitedRetTypes...>, RetTypes...> await_transform(Coroutine<AwaitedRetTypes...> h) {
        return ChainedAwaitObject<Coroutine<AwaitedRetTypes...>, RetTypes...>(std::move(h));
    }

    template<class... AwaitedRetTypes>
    GetCoroutineHandleAwaitObject<AwaitedRetTypes...> await_transform(GetCoroutineHandleAwaitObject<AwaitedRetTypes...> h) {
        return std::move(h);
    }

    template<class... AwaitedRetTypes>
    SuspendThisCoroutineAwaitObject<AwaitedRetTypes...> await_transform(SuspendThisCoroutineAwaitObject<AwaitedRetTypes...> h) {
        return std::move(h);
    }

    std::suspend_never initial_suspend() noexcept {
        return {};
    }

    FinalAwait final_suspend() noexcept {
        return {nextCoro};
    }

    void unhandled_exception() {
        auto a = std::current_exception();
        std::abort();
    }

    void setNextCoro(void *coro) {
        nextCoro = coro;
    }
};

template<class... RetTypes>
class Promise : public PromiseBase<RetTypes...> {
public:
    std::tuple<RetTypes...> val;

    Coroutine<RetTypes...> get_return_object() {
        return Coroutine<RetTypes...>(std::coroutine_handle<Promise<RetTypes...>>::from_promise(*this));
    }

    void return_value(std::tuple<RetTypes...> v) {
        val = std::move(v);
    }
};

template<class RetType>
class Promise<RetType> : public PromiseBase<RetType> {
public:
    RetType val;

    Coroutine<RetType> get_return_object() {
        return Coroutine<RetType>(std::coroutine_handle<Promise<RetType>>::from_promise(*this));
    }

    void return_value(RetType v) {
        val = std::move(v);
    }
};

template<>
class Promise<> : public PromiseBase<> {
public:
    Coroutine<> get_return_object() {
        return Coroutine<>(std::coroutine_handle<Promise<>>::from_promise(*this));
    }

    void return_void() {
    }
};


}// namespace RingSwarm::async


#endif//RINGSWARM_SRC_ASYNC_PROMISE_H
