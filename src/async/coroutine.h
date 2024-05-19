#ifndef RINGSWARM_SRC_ASYNC_COROUTINE_H
#define RINGSWARM_SRC_ASYNC_COROUTINE_H

namespace RingSwarm::async {
template<class...>
class Coroutine;

template<class...>
class ChainedAwaitObject;

template<class...>
class Promise;

template<class...>
class GetCoroutineHandleAwaitObject;

template<class...>
class SuspendThisCoroutineAwaitObject;

template<class...>
class AwaitResumeResult;

template<class...>
class ResumeSuspendAwaitObject;
}// namespace RingSwarm::async

#include <coroutine>

template<class... RetValues, class... ArgTypes>
struct std::
    coroutine_traits<RingSwarm::async::Coroutine<RetValues...>, ArgTypes...> {
    using promise_type = RingSwarm::async::Promise<RetValues...>;
};

#include "AwaitResumeResult.h"
#include "ChainedAwaitObject.h"
#include "Coroutine.h"
#include "FinalAwait.h"
#include "GetCoroutineHandleAwaitObject.h"
#include "Promise.h"
#include "ResumeSuspendAwaitObject.h"
#include "SuspendThisCoroutineAwaitObject.h"

// separate impl header
#include "ChainedAwaitObjectImpl.h"

#endif// RINGSWARM_SRC_ASYNC_COROUTINE_H
