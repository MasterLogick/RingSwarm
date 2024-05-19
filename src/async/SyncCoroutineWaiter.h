#ifndef RINGSWARM_SRC_ASYNC_SYNCCOROUTINEWAITER_H
#define RINGSWARM_SRC_ASYNC_SYNCCOROUTINEWAITER_H

#include "coroutine.h"

namespace RingSwarm::async {
template<typename...>
class SyncCoroutineWaiter;

template<typename... RetTypes>
class SyncCoroutineWaiter<Coroutine<RetTypes...>> {
    async::Coroutine<> waitHelper(
        async::Coroutine<RetTypes...> coroutine,
        std::tuple<RetTypes...> &retValues,
        std::condition_variable &cv,
        bool &doneFlag
    ) {
        try {
            retValues = static_cast<std::tuple<RetTypes...>>(
                co_await std::move(coroutine)
            );
            doneFlag = true;
            cv.notify_one();
        } catch (...) {
            doneFlag = true;
            cv.notify_one();
            throw;
        }
    }

public:
    std::tuple<RetTypes...> wait(async::Coroutine<RetTypes...> coroutine) {
        std::tuple<RetTypes...> retValues;
        std::condition_variable cv;
        bool doneFlag = false;
        auto h = waitHelper(std::move(coroutine), retValues, cv, doneFlag);
        std::mutex m;
        std::unique_lock<std::mutex> lk(m);
        cv.wait(lk, [&doneFlag] { return doneFlag; });
        h.getHandle().promise().check();
        return retValues;
    }
};

template<typename RetType>
class SyncCoroutineWaiter<Coroutine<RetType>> {
    async::Coroutine<> waitHelper(
        async::Coroutine<RetType> coroutine,
        RetType *retValue,
        std::condition_variable &cv,
        bool &doneFlag
    ) {
        try {
            *retValue = co_await std::move(coroutine);
            doneFlag = true;
            cv.notify_one();
        } catch (...) {
            doneFlag = true;
            cv.notify_one();
            throw;
        }
    }

public:
    std::tuple<RetType> wait(async::Coroutine<RetType> coroutine) {
        std::tuple<RetType> retValues;
        std::exception_ptr ptr;
        std::condition_variable cv;
        bool doneFlag = false;
        auto h = waitHelper(std::move(coroutine), retValues, cv, doneFlag);
        std::mutex m;
        std::unique_lock<std::mutex> lk(m);
        cv.wait(lk, [&doneFlag] { return doneFlag; });
        h.getHandle().promise().check();
        return retValues;
    }
};

template<>
class SyncCoroutineWaiter<Coroutine<>> {
    async::Coroutine<> waitHelper(
        async::Coroutine<> coroutine,
        std::condition_variable &cv,
        bool &doneFlag
    ) {
        try {
            ~co_await std::move(coroutine);
            doneFlag = true;
            cv.notify_one();
        } catch (...) {
            doneFlag = true;
            cv.notify_one();
            throw;
        }
    }

public:
    void wait(async::Coroutine<> coroutine) {
        std::condition_variable cv;
        bool doneFlag = false;
        auto h = waitHelper(std::move(coroutine), cv, doneFlag);
        std::mutex m;
        std::unique_lock<std::mutex> lk(m);
        cv.wait(lk, [&doneFlag] { return doneFlag; });
        h.getHandle().promise().check();
    }
};

} // namespace RingSwarm::async

#endif // RINGSWARM_SRC_ASYNC_SYNCCOROUTINEWAITER_H
