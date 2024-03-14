#ifndef RINGSWARM_SRC_ASYNC_PROMISEIMPL_H
#define RINGSWARM_SRC_ASYNC_PROMISEIMPL_H

namespace RingSwarm::async {
template<class RetType>
template<class AwaitedRetType>
ChainedAwaitObject<AwaitedRetType, RetType> PromiseBase<RetType>::await_transform(Coroutine<AwaitedRetType> h) {
    std::cout << "coroutine promise await transform" << std::endl;
    return ChainedAwaitObject<AwaitedRetType, RetType>(std::move(h));
}

template<class RetType>
Coroutine<RetType> Promise<RetType>::get_return_object() {
    std::cout << "coroutine promise get_return_object" << std::endl;
    return Coroutine<RetType>(std::coroutine_handle<Promise<RetType>>::from_promise(*this));
}

Coroutine<void> Promise<void>::get_return_object() {
    std::cout << "coroutine promise get_return_object" << std::endl;
    return Coroutine<void>(std::coroutine_handle<Promise<void>>::from_promise(*this));
}
}// namespace RingSwarm::async
#endif//RINGSWARM_SRC_ASYNC_PROMISEIMPL_H
