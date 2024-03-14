#ifndef RINGSWARM_SRC_ASYNC_COROUTINE_H
#define RINGSWARM_SRC_ASYNC_COROUTINE_H

namespace RingSwarm::async {
template<class = void>
class Coroutine;

template<class = void, class = void>
class ChainedAwaitObject;

template<class = void>
class Promise;
}// namespace RingSwarm::async

#include "ChainedAwaitObject.h"
#include "Coroutine.h"
#include "Promise.h"
#include "FinalAwait.h"

#include "ChainedAwaitObjectImpl.h"
#include "PromiseImpl.h"

template<class RetValue, class... ArgTypes>
struct std::coroutine_traits<RingSwarm::async::Coroutine<RetValue>, ArgTypes...> {
    using promise_type = RingSwarm::async::Promise<RetValue>;
};

#endif//RINGSWARM_SRC_ASYNC_COROUTINE_H
