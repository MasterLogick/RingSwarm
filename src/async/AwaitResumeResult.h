#include <utility>

#ifndef RINGSWARM_SRC_ASYNC_AWAITRESUMERESULT_H
#define RINGSWARM_SRC_ASYNC_AWAITRESUMERESULT_H


namespace RingSwarm::async {
template<typename...>
class AwaitResumeResultBase;

template<typename... AwaitedRetTypes, typename... RetTypes>
class AwaitResumeResultBase<Coroutine<AwaitedRetTypes...>, RetTypes...> {
protected:
    std::exception_ptr exceptionPtr;
    bool touched = false;

public:
    explicit AwaitResumeResultBase(std::exception_ptr exceptionPtr) : exceptionPtr(std::move(exceptionPtr)) {
    }

    AwaitResumeResultBase(const AwaitResumeResultBase<Coroutine<AwaitedRetTypes...>, RetTypes...> &) = delete;

    AwaitResumeResultBase(AwaitResumeResultBase<Coroutine<AwaitedRetTypes...>, RetTypes...> &&) = delete;

    void touch() {
        touched = true;
        if (exceptionPtr) {
            std::rethrow_exception(exceptionPtr);
        }
    }

    void operator~() {
        touch();
    }

    ~AwaitResumeResultBase() {
        Assert(touched, "co_await return value must be touched before destruction");
    }
};

template<typename...>
class AwaitResumeResult;

template<typename... AwaitedRetTypes, typename... RetTypes>
class AwaitResumeResult<Coroutine<AwaitedRetTypes...>, RetTypes...> : public AwaitResumeResultBase<Coroutine<AwaitedRetTypes...>, RetTypes...> {
    std::tuple<AwaitedRetTypes...> retValues;

public:
    explicit AwaitResumeResult(std::tuple<AwaitedRetTypes...> retValues, std::exception_ptr exceptionPtr)
        : AwaitResumeResultBase<Coroutine<AwaitedRetTypes...>, RetTypes...>(std::move(exceptionPtr)), retValues(std::move(retValues)) {}

    template<std::size_t I>
    auto get() {
        AwaitResumeResultBase<Coroutine<AwaitedRetTypes...>, RetTypes...>::touch();
        return std::move(std::get<I>(retValues));
    }

    operator std::tuple<AwaitedRetTypes...>() {
        AwaitResumeResultBase<Coroutine<AwaitedRetTypes...>, RetTypes...>::touch();
        return std::move(retValues);
    }
};

template<class AwaitedRetType, class... RetTypes>
class AwaitResumeResult<Coroutine<AwaitedRetType>, RetTypes...> : public AwaitResumeResultBase<Coroutine<AwaitedRetType>, RetTypes...> {
    AwaitedRetType retValue;

public:
    explicit AwaitResumeResult(AwaitedRetType retValue, std::exception_ptr exceptionPtr)
        : AwaitResumeResultBase<Coroutine<AwaitedRetType>, RetTypes...>(std::move(exceptionPtr)), retValue(std::move(retValue)) {}

    operator AwaitedRetType() {
        AwaitResumeResultBase<Coroutine<AwaitedRetType>, RetTypes...>::touch();
        return std::move(retValue);
    }
};

template<class... RetTypes>
class AwaitResumeResult<Coroutine<>, RetTypes...> : public AwaitResumeResultBase<Coroutine<>, RetTypes...> {
public:
    explicit AwaitResumeResult(std::exception_ptr exceptionPtr)
        : AwaitResumeResultBase<Coroutine<>, RetTypes...>(std::move(exceptionPtr)) {}
};

}// namespace RingSwarm::async

template<typename... AwaitedRetTypes, typename... RetTypes>
struct std::tuple_size<RingSwarm::async::AwaitResumeResult<RingSwarm::async::Coroutine<AwaitedRetTypes...>, RetTypes...>>
    : public std::tuple_size<std::tuple<AwaitedRetTypes...>> {};

template<std::size_t I, typename... AwaitedRetTypes, typename... RetTypes>
struct std::tuple_element<I, RingSwarm::async::AwaitResumeResult<RingSwarm::async::Coroutine<AwaitedRetTypes...>, RetTypes...>> {
    using type = std::tuple_element<I, std::tuple<AwaitedRetTypes...>>::type;
};

#endif//RINGSWARM_SRC_ASYNC_AWAITRESUMERESULT_H
