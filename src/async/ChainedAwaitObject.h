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
    Coroutine<AwaitedRetTypes...> c;

public:
    explicit ChainedAwaitObjectBase(Coroutine<AwaitedRetTypes...> c) : c(std::move(c)) {
        std::cout << "chained await object constructor" << std::endl;
    }

    bool await_ready() noexcept {
        std::cout << "chained await object await_ready" << std::endl;
        return c.getHandle().done();
    }

    void await_suspend(std::coroutine_handle<Promise<RetTypes...>> h) {
        std::cout << "chained await object await_suspend" << std::endl;
        c.getHandle().promise().setNextCoro(h.address());
    }
};

template<typename...>
class ChainedAwaitObject;

template<typename... AwaitedRetTypes, typename... RetTypes>
class ChainedAwaitObject<Coroutine<AwaitedRetTypes...>, RetTypes...> : public ChainedAwaitObjectBase<Coroutine<AwaitedRetTypes...>, RetTypes...> {
public:
    ChainedAwaitObject(Coroutine<AwaitedRetTypes...> c) : ChainedAwaitObjectBase<Coroutine<AwaitedRetTypes...>, RetTypes...>(std::move(c)) {}

    std::tuple<AwaitedRetTypes...> await_resume() noexcept {
        std::cout << "chained await object await_resume" << std::endl;
        return ChainedAwaitObjectBase<Coroutine<AwaitedRetTypes...>, RetTypes...>::c.getHandle().promise().val;
    }
};


template<class AwaitedRetType, class... RetTypes>
class ChainedAwaitObject<Coroutine<AwaitedRetType>, RetTypes...> : public ChainedAwaitObjectBase<Coroutine<AwaitedRetType>, RetTypes...> {
public:
    ChainedAwaitObject(Coroutine<AwaitedRetType> c) : ChainedAwaitObjectBase<Coroutine<AwaitedRetType>, RetTypes...>(std::move(c)) {}

    AwaitedRetType await_resume() noexcept {
        std::cout << "chained await object await_resume" << std::endl;
        return ChainedAwaitObjectBase<Coroutine<AwaitedRetType>, RetTypes...>::c.getHandle().promise().val;
    }
};

template<class... RetTypes>
class ChainedAwaitObject<Coroutine<>, RetTypes...> : public ChainedAwaitObjectBase<Coroutine<>, RetTypes...> {
public:
    ChainedAwaitObject(Coroutine<> c);

    void await_resume() noexcept {
        std::cout << "chained await object await_resume" << std::endl;
    }
};
}// namespace RingSwarm::async
#endif//COROUTINES_CHAINEDChainedAwaitObject_H
