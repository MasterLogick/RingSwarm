#ifndef RINGSWARM_SRC_ASYNC_RESUMESUSPENDAWAITOBJECT_H
#define RINGSWARM_SRC_ASYNC_RESUMESUSPENDAWAITOBJECT_H

namespace RingSwarm::async {

template<typename... RetTypes>
class ResumeSuspendAwaitObject {
    Coroutine<RetTypes...> coroutine;

public:
    explicit ResumeSuspendAwaitObject(Coroutine<RetTypes...> coroutine)
        : coroutine(std::move(coroutine)) {}

    Coroutine<RetTypes...> getCoroutine() {
        return std::move(coroutine);
    }
};

} // namespace RingSwarm::async

#endif // RINGSWARM_SRC_ASYNC_RESUMESUSPENDAWAITOBJECT_H
