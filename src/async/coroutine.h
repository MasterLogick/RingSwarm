#ifndef RINGSWARM_SRC_ASYNC_COROUTINE_H
#define RINGSWARM_SRC_ASYNC_COROUTINE_H

namespace RingSwarm::async {
template<class...>
class Coroutine;

template<class...>
class ChainedAwaitObject;

template<class...>
class Promise;
}// namespace RingSwarm::async

#include "ChainedAwaitObject.h"
#include "Coroutine.h"
#include "FinalAwait.h"
#include "Promise.h"

#include "ChainedAwaitObjectImpl.h"

template<class... RetValues, class... ArgTypes>
struct std::coroutine_traits<RingSwarm::async::Coroutine<RetValues...>, ArgTypes...> {
    using promise_type = RingSwarm::async::Promise<RetValues...>;
};

#endif//RINGSWARM_SRC_ASYNC_COROUTINE_H
