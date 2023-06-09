#ifndef RINGSWARM_SPINLOCK_H
#define RINGSWARM_SPINLOCK_H

#include <atomic>

namespace RingSwarm::async {
    class Spinlock {
        std::atomic_flag flag;
    public:
        Spinlock() : flag(false) {}

        void lock() {
            while (flag.test_and_set());
        }

        void unlock() {
            flag.clear();
        }
    };

} // async

#endif //RINGSWARM_SPINLOCK_H
