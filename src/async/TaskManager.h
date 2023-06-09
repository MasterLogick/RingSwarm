#ifndef RINGSWARM_TASKMANAGER_H
#define RINGSWARM_TASKMANAGER_H

#include <functional>

namespace RingSwarm::async {
    void addTask(const std::function<void(void)> &task);

    void runTaskHandlers(int threadCount);
}

#endif //RINGSWARM_TASKMANAGER_H
