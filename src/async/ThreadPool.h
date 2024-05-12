#ifndef RINGSWARM_TASKMANAGER_H
#define RINGSWARM_TASKMANAGER_H

#include <condition_variable>
#include <shared_mutex>
#include <thread>
#include <vector>

namespace RingSwarm::async {
class ThreadPool {
    static ThreadPool *defaultThreadPool;

    std::vector<std::thread> pool;
    std::vector<void *> tasks;
    std::mutex popLock;
    std::condition_variable popCondition;
    std::mutex noTasksMutex;
    std::condition_variable noTasksCondition;
    bool stopFlag = false;

public:
    static ThreadPool *getDefaultThreadPool() {
        return defaultThreadPool;
    }

    static void setDefaultThreadPool(ThreadPool *pool) {
        defaultThreadPool = pool;
    }

    explicit ThreadPool(unsigned int threadNum);

    void resumeCoroutine(void *handle);

    void waitEmpty();

    void blockingStop();

    bool isScheduled(void *address);
};
}// namespace RingSwarm::async

#endif// RINGSWARM_TASKMANAGER_H
