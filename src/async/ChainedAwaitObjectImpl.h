#ifndef RINGSWARM_SRC_ASYNC_CHAINEDAWAITOBJECTIMPL_H
#define RINGSWARM_SRC_ASYNC_CHAINEDAWAITOBJECTIMPL_H

namespace RingSwarm::async {
template<class RetType>
ChainedAwaitObject<void, RetType>::ChainedAwaitObject(Coroutine<void> c) : ChainedAwaitObjectBase<void, RetType>(std::move(c)) {}
}// namespace RingSwarm::async

#endif//RINGSWARM_SRC_ASYNC_CHAINEDAWAITOBJECTIMPL_H
