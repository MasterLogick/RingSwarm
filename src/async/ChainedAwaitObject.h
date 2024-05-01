#ifndef COROUTINES_CHAINEDChainedAwaitObject_H
#define COROUTINES_CHAINEDChainedAwaitObject_H

#include <coroutine>
#include <iostream>

namespace RingSwarm::async {
template<typename...>
class ChainedAwaitObjectBase;

template<typename... AwaitedRetTypes, typename... RetTypes>
class ChainedAwaitObjectBase<Coroutine<AwaitedRetTypes...>, RetTypes...> {
protected:
    Coroutine<AwaitedRetTypes...> innerCoroutine;

public:
    explicit ChainedAwaitObjectBase(Coroutine<AwaitedRetTypes...> innerCoroutine) : innerCoroutine(std::move(innerCoroutine)) {
    }

    bool await_ready() noexcept {
        return innerCoroutine.getHandle().done();
    }

    void await_suspend(std::coroutine_handle<Promise<RetTypes...>> h) {
        innerCoroutine.getHandle().promise().setCallerCoroutine(h);
    }
};

template<typename...>
class ChainedAwaitObject;

template<typename... AwaitedRetTypes, typename... RetTypes>
class ChainedAwaitObject<Coroutine<AwaitedRetTypes...>, RetTypes...> : public ChainedAwaitObjectBase<Coroutine<AwaitedRetTypes...>, RetTypes...> {
public:
    explicit ChainedAwaitObject(Coroutine<AwaitedRetTypes...> innerCoroutine) : ChainedAwaitObjectBase<Coroutine<AwaitedRetTypes...>, RetTypes...>(std::move(innerCoroutine)) {}

    AwaitResumeResult<Coroutine<AwaitedRetTypes...>, RetTypes...> await_resume() noexcept {
        auto &promise = ChainedAwaitObjectBase<Coroutine<AwaitedRetTypes...>, RetTypes...>::innerCoroutine.getHandle().promise();
        return AwaitResumeResult<Coroutine<AwaitedRetTypes...>, RetTypes...>(std::move(promise.moveValue()), promise.getAndClearExceptionPtr());
    }
};

template<class AwaitedRetType, class... RetTypes>
class ChainedAwaitObject<Coroutine<AwaitedRetType>, RetTypes...> : public ChainedAwaitObjectBase<Coroutine<AwaitedRetType>, RetTypes...> {
public:
    explicit ChainedAwaitObject(Coroutine<AwaitedRetType> innerCoroutine) : ChainedAwaitObjectBase<Coroutine<AwaitedRetType>, RetTypes...>(std::move(innerCoroutine)) {}

    AwaitResumeResult<Coroutine<AwaitedRetType>, RetTypes...> await_resume() noexcept {
        auto &promise = ChainedAwaitObjectBase<Coroutine<AwaitedRetType>, RetTypes...>::innerCoroutine.getHandle().promise();
        return AwaitResumeResult<Coroutine<AwaitedRetType>, RetTypes...>(std::move(promise.moveValue()), promise.getAndClearExceptionPtr());
    }
};

template<class... RetTypes>
class ChainedAwaitObject<Coroutine<>, RetTypes...> : public ChainedAwaitObjectBase<Coroutine<>, RetTypes...> {
public:
    explicit ChainedAwaitObject(Coroutine<> innerCoroutine);

    AwaitResumeResult<Coroutine<>, RetTypes...> await_resume() noexcept {
        auto &promise = ChainedAwaitObjectBase<Coroutine<>, RetTypes...>::innerCoroutine.getHandle().promise();
        return AwaitResumeResult<Coroutine<>, RetTypes...>(promise.getAndClearExceptionPtr());
    }
};
}// namespace RingSwarm::async
#endif//COROUTINES_CHAINEDChainedAwaitObject_H
