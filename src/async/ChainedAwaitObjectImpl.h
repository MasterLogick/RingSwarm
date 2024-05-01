#ifndef RINGSWARM_SRC_ASYNC_CHAINEDAWAITOBJECTIMPL_H
#define RINGSWARM_SRC_ASYNC_CHAINEDAWAITOBJECTIMPL_H

namespace RingSwarm::async {
template<class... RetTypes>
ChainedAwaitObject<Coroutine<>, RetTypes...>::ChainedAwaitObject(Coroutine<> c)
    : ChainedAwaitObjectBase<Coroutine<>, RetTypes...>(std::move(c)) {}
}// namespace RingSwarm::async

#endif// RINGSWARM_SRC_ASYNC_CHAINEDAWAITOBJECTIMPL_H
