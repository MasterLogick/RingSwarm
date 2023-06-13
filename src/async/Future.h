#ifndef RINGSWARM_FUTURE_H
#define RINGSWARM_FUTURE_H

#include <functional>
#include <atomic>
#include <condition_variable>
#include "TaskManager.h"
#include "Spinlock.h"
#include <boost/log/trivial.hpp>

namespace RingSwarm::async {
    template<class ...Args>
    class Future;

    template<>
    class Future<void> : public std::enable_shared_from_this<Future<void>> {
        std::vector<std::function<void()>> callbacks;
        Spinlock accessSpinlock;
        volatile bool resolved;

        Future() : resolved(false) {}

        void then1(const std::function<void()> &t) {
            accessSpinlock.lock();
            if (resolved) {
                accessSpinlock.unlock();
                addTask(t);
            } else {
                callbacks.push_back(t);
                accessSpinlock.unlock();
            }
        }

    public:
        static std::shared_ptr<Future<void>> create() {
            return std::shared_ptr<Future<void>>(new Future<void>());
        }

        static std::shared_ptr<Future<void>> createResolved() {
            auto x = Future<void>::create();
            x->resolve();
            return x;
        }

        template<class T>
        std::shared_ptr<Future<T>> then(const std::function<T()> &t) {
            std::shared_ptr<Future<T>> a = Future<T>::create();
            //todo maybe copy function by value to local scope if lambda copies by reference
            then1([a, t]() {
                a->resolve(std::forward<T>(t()));
            });
            return a;
        }

        template<class T>
        std::shared_ptr<Future<T>> then(const std::function<std::shared_ptr<Future<T>>()> &t) {
            std::shared_ptr<Future<T>> a = Future<T>::create();
            //todo maybe copy function by value to local scope if lambda copies by reference
            then1([a, t]() {
                t()->then([a](T x) {
                    a->resolve(std::forward<T>(x));
                });
            });
            return a;
        }

        std::shared_ptr<Future<void>> then(const std::function<void()> &t) {
            std::shared_ptr<Future<void>> a = Future<void>::create();
            //todo maybe copy function by value to local scope if lambda copies by reference
            then1([a, t]() {
                t();
                a->resolve();
            });
            return a;
        }

        void resolve() {
//            BOOST_LOG_TRIVIAL(trace) << "resolved " << this;
            accessSpinlock.lock();
            if (resolved) {
                accessSpinlock.unlock();
                //todo throw second resolve exception
            } else {
                resolved = true;
                accessSpinlock.unlock();
                for (const std::function<void(void)> &item: callbacks) {
                    addTask(item);
                }
                callbacks.clear();
            }
        }

        void await() {
            if (!resolved) {
                std::mutex m;
                std::condition_variable cv;
                std::unique_lock<std::mutex> ul(m);
                then1([&m, &cv, this]() {
                    std::lock_guard<std::mutex> l(m);
                    cv.notify_one();
//                    BOOST_LOG_TRIVIAL(trace) << "notified " << this;
                });
//                BOOST_LOG_TRIVIAL(trace) << "wait " << this;
                cv.wait(ul, [this]() { return resolved; });
//                BOOST_LOG_TRIVIAL(trace) << "waited " << this;
            }
        }

        bool isResolved() {
            return resolved;
        }
    };

    template<class ...Args>
    class Future : public std::enable_shared_from_this<Future<Args...>> {
        std::vector<std::function<void(Args...)>> callbacks;
        Spinlock accessSpinlock;
        volatile bool resolved;
        std::tuple<Args...> resolvedValues;

        Future() : resolved(false) {}

        void then1(const std::function<void(Args...)> &t) {
            accessSpinlock.lock();
            if (resolved) {
                accessSpinlock.unlock();
                std::shared_ptr<Future<Args...>> ptr = this->shared_from_this();
                addTask([ptr, t]() {
                    std::apply(t, ptr->resolvedValues);
                });
            } else {
                callbacks.push_back(t);
                accessSpinlock.unlock();
            }
        }

    public:
        static std::shared_ptr<Future<Args...>> create() {
            return std::shared_ptr<Future<Args...>>(new Future<Args...>());
        }

        static std::shared_ptr<Future<Args...>> createResolved(Args... args) {
            auto x = Future<Args...>::create();
            x->resolve(std::forward<Args>(args)...);
            return x;
        }

        template<class T>
        std::shared_ptr<Future<T>> then(const std::function<T(Args...)> &t) {
            std::shared_ptr<Future<T>> a = Future<T>::create();
            //todo maybe copy function by value to local scope if lambda copies by reference
            then1([a, t](Args... args) {
                a->resolve(std::forward<T>(t(std::forward<Args>(args)...)));
            });
            return a;
        }

        std::shared_ptr<Future<void>> then(const std::function<void(Args...)> &t) {
            std::shared_ptr<Future<void>> a = Future<void>::create();
            //todo maybe copy function by value to local scope if lambda copies by reference
            then1([a, t](Args... args) {
                t(std::forward<Args>(args)...);
                a->resolve();
            });
            return a;
        }

        template<class T>
        std::shared_ptr<Future<T>> then(const std::function<std::shared_ptr<Future<T>>(Args...)> &t) {
            std::shared_ptr<Future<T>> a = Future<T>::create();
            //todo maybe copy function by value to local scope if lambda copies by reference
            then1([a, t](Args... args) {
                t(std::forward<Args>(args)...)->then([a](T x) {
                    a->resolve(std::forward<T>(x));
                });
            });
            return a;
        }

        void resolve(Args... args) {
//            BOOST_LOG_TRIVIAL(trace) << "resolved " << this;
            accessSpinlock.lock();
            if (resolved) {
                accessSpinlock.unlock();
                // ignore further resolves
            } else {
                resolved = true;
                resolvedValues = std::make_tuple(std::forward<Args>(args)...);
                accessSpinlock.unlock();
                for (const std::function<void(Args...)> &item: callbacks) {
                    std::shared_ptr<Future<Args...>> ptr = this->shared_from_this();
                    addTask([ptr, item]() {
                        std::apply(item, ptr->resolvedValues);
                    });
                }
                callbacks.clear();
            }
        }

        std::tuple<Args...> await() {
            if (!resolved) {
                std::mutex m;
                std::condition_variable cv;
                std::unique_lock<std::mutex> ul(m);
                then1([&m, &cv, this](Args... args) {
                    std::lock_guard<std::mutex> l(m);
                    cv.notify_one();
//                    BOOST_LOG_TRIVIAL(trace) << "notified " << this;
                });
//                BOOST_LOG_TRIVIAL(trace) << "wait " << this;
                cv.wait(ul, [this]() { return resolved; });
//                BOOST_LOG_TRIVIAL(trace) << "waited " << this;
                return resolvedValues;
            }
            return resolvedValues;
        }

        bool isResolved() {
            return resolved;
        }
    };

    template<class Iter, class ...Args>
    std::shared_ptr<Future<Args...>> waitForAny(Iter begin, Iter end) {
        auto f = Future<Args...>::create();
        for (; begin != end; begin++) {
            auto x = *begin;
            x->then([f](Args... args) {
                f->resolve(std::forward<Args>(args)...);
            });
        }
        return f;
    }
}

#endif //RINGSWARM_FUTURE_H
