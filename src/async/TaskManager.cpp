#include "TaskManager.h"

#include "../core/Thread.h"

#include <condition_variable>
#include <list>
#include <mutex>
#include <queue>
#include <thread>

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
        std::thread([](int i) {
            core::setThreadName(("Task runner " + std::to_string(i)).c_str());
            while (true) {
                std::unique_lock<std::mutex> l(taskReadyLock);
                condVar.wait(l, [] { return !readyTasks.empty(); });
                auto taskInstance = readyTasks.front();
                readyTasks.pop();
                l.unlock();
                //todo try catch exceptions
                taskInstance();
            }
        },
                    i)
                .detach();
    }
}
}// namespace RingSwarm::async