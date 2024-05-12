#ifndef RINGSWARM_SRC_ASYNC_PROMISE_H
#define RINGSWARM_SRC_ASYNC_PROMISE_H

#include "../Assert.h"

namespace RingSwarm::async {
template<class... RetTypes>
class PromiseBase {
    std::coroutine_handle<> callerCoroutine;
    std::exception_ptr exceptionPtr;

    void handleUnhandledException(const std::exception_ptr &ptr) {
        if (callerCoroutine == nullptr) {
            std::rethrow_exception(ptr);
        } else {
            Assert(exceptionPtr == nullptr, "Double fault");
            exceptionPtr = ptr;
        }
    }

public:
    template<class... AwaitedRetTypes>
    ChainedAwaitObject<Coroutine<AwaitedRetTypes...>, RetTypes...>
    await_transform(Coroutine<AwaitedRetTypes...> &&h) {
        return ChainedAwaitObject<Coroutine<AwaitedRetTypes...>, RetTypes...>(
            std::move(h)
        );
    }

    template<class... AwaitedRetTypes>
    GetCoroutineHandleAwaitObject<AwaitedRetTypes...>
    await_transform(GetCoroutineHandleAwaitObject<AwaitedRetTypes...> h) {
        return std::move(h);
    }

    template<class... AwaitedRetTypes>
    SuspendThisCoroutineAwaitObject<AwaitedRetTypes...>
    await_transform(SuspendThisCoroutineAwaitObject<AwaitedRetTypes...> h) {
        return std::move(h);
    }

    std::suspend_never initial_suspend() noexcept {
        return {};
    }

    FinalAwait final_suspend() noexcept {
        return {callerCoroutine};
    }

    void unhandled_exception() {
        handleUnhandledException(std::current_exception());
    }

    void raiseException(auto ex) {
        handleUnhandledException(std::make_exception_ptr(ex));
    }

    std::exception_ptr getAndClearExceptionPtr() {
        std::exception_ptr d = exceptionPtr;
        exceptionPtr = nullptr;
        return d;
    }

    void setCallerCoroutine(std::coroutine_handle<> caller) {
        Assert(callerCoroutine == nullptr, "double join");
        callerCoroutine = caller;
    }

    void check() {
        auto ptr = getAndClearExceptionPtr();
        if (ptr) {
            std::rethrow_exception(ptr);
        }
    }

    ~PromiseBase() {
        Assert(!exceptionPtr, "unhandled exception on promise destroy");
        void *addr =
            std::coroutine_handle<PromiseBase<RetTypes...>>::from_promise(*this)
                .address();
        Assert(
            !ThreadPool::getDefaultThreadPool()->isScheduled(addr),
            "promise is scheduled on destroy"
        );
    }
};

template<class... RetTypes>
class Promise : public PromiseBase<RetTypes...> {
    std::tuple<RetTypes...> val;

public:
    Coroutine<RetTypes...> get_return_object() {
        return Coroutine<RetTypes...>(
            std::coroutine_handle<Promise<RetTypes...>>::from_promise(*this)
        );
    }

    void return_value(std::tuple<RetTypes...> v) {
        val = std::move(v);
    }

    std::tuple<RetTypes...> &&moveValue() {
        return std::move(val);
    }
};

template<class RetType>
class Promise<RetType> : public PromiseBase<RetType> {
    RetType val;

public:
    Coroutine<RetType> get_return_object() {
        return Coroutine<RetType>(
            std::coroutine_handle<Promise<RetType>>::from_promise(*this)
        );
    }

    void return_value(RetType v) {
        val = std::move(v);
    }

    RetType &&moveValue() {
        return std::move(val);
    }
};

template<>
class Promise<> : public PromiseBase<> {
public:
    Coroutine<> get_return_object() {
        return Coroutine<>(std::coroutine_handle<Promise<>>::from_promise(*this)
        );
    }

    void return_void() {}
};

}// namespace RingSwarm::async

#endif// RINGSWARM_SRC_ASYNC_PROMISE_H
