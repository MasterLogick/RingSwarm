#ifndef COROUTINES_CHAINEDChainedAwaitObject_H
#define COROUTINES_CHAINEDChainedAwaitObject_H

#include <coroutine>
#include <iostream>

namespace RingSwarm::async {
template<class AwaitedRetType, class RetType>
class ChainedAwaitObjectBase {
protected:
    Coroutine<AwaitedRetType> c;

public:
    explicit ChainedAwaitObjectBase(Coroutine<AwaitedRetType> c) : c(std::move(c)) {
        std::cout << "chained await object constructor" << std::endl;
    }

    bool await_ready() noexcept {
        std::cout << "chained await object await_ready" << std::endl;
        return c.getHandle().done();
    }

    void await_suspend(std::coroutine_handle<Promise<RetType>> h) {
        std::cout << "chained await object await_suspend" << std::endl;
        c.getHandle().promise().setNextCoro(h.address());
    }
};

template<class AwaitedRetType, class RetType>
class ChainedAwaitObject : public ChainedAwaitObjectBase<AwaitedRetType, RetType> {
public:
    ChainedAwaitObject(Coroutine<AwaitedRetType> c) : ChainedAwaitObjectBase<AwaitedRetType, RetType>(std::move(c)) {}

    AwaitedRetType await_resume() noexcept {
        std::cout << "chained await object await_resume" << std::endl;
        return ChainedAwaitObjectBase<AwaitedRetType, RetType>::c.getHandle().promise().val;
    }
};

template<class RetType>
class ChainedAwaitObject<void, RetType> : public ChainedAwaitObjectBase<void, RetType> {
public:
    ChainedAwaitObject(Coroutine<void> c);

    void await_resume() noexcept {
        std::cout << "chained await object await_resume" << std::endl;
    }
};

}// namespace RingSwarm::async
#endif//COROUTINES_CHAINEDChainedAwaitObject_H
