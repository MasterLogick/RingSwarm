#include "ThreadPool.h"
#include "../core/Thread.h"
#include <coroutine>
#include <iostream>

namespace RingSwarm::async {
ThreadPool *ThreadPool::defaultThreadPool = nullptr;
ThreadPool::ThreadPool(unsigned int threadNum) {
    for (unsigned int i = 0; i < threadNum; ++i) {
        pool.emplace_back([this, i] {
            std::string threadName = "RingSwarm - worker thread " + std::to_string(i);
            core::setThreadName(threadName.c_str());
            while (!stopFlag) {
                void *task;
                {
                    std::unique_lock<std::mutex> lk(popLock);
                    popCondition.wait(lk, [this] { return !tasks.empty(); });
                    task = tasks.back();
                    tasks.pop_back();
                    if (tasks.empty()) noTasksCondition.notify_all();
                }
                if (task) std::coroutine_handle<>::from_address(task).resume();
            }
        });
    }
}

void ThreadPool::resumeCoroutine(void *handle) {
    {
        std::lock_guard<std::mutex> _(popLock);
        tasks.push_back(handle);
    }
    popCondition.notify_one();
}

void ThreadPool::waitEmpty() {
    std::unique_lock<std::mutex> lk(noTasksMutex);
    noTasksCondition.wait(lk, [&] { return tasks.empty(); });
}

void ThreadPool::blockingStop() {
    stopFlag = true;
    for (int i = 0; i < pool.size(); ++i) {
        resumeCoroutine(nullptr);
    }
    for (auto &thr: pool) {
        thr.join();
    }
}
}// namespace RingSwarm::async