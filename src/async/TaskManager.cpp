#include "TaskManager.h"
#include <list>
#include <thread>
#include <mutex>
#include <queue>
#include <functional>
#include <condition_variable>

namespace RingSwarm::async {
    std::queue<std::function<void()>> readyTasks;
    std::mutex taskReadyLock;
    std::condition_variable condVar;

    void addTask(const std::function<void()> &task) {
        std::lock_guard<std::mutex> l(taskReadyLock);
        readyTasks.push(task);
        condVar.notify_one();
    }

    void runTaskHandlers(int threadCount) {
        for (int i = 0; i < threadCount; ++i) {
            std::thread([] {
                while (true) {
                    std::unique_lock<std::mutex> l(taskReadyLock);
                    condVar.wait(l, [] { return !readyTasks.empty(); });
                    auto taskInstance = readyTasks.front();
                    readyTasks.pop();
                    l.unlock();
                    //todo try catch exceptions
                    taskInstance();
                }
            }).detach();
        }
    }
}