#include "Thread.h"
#include <pthread.h>

namespace RingSwarm::core {
void setThreadName(const char *name) {
    pthread_setname_np(pthread_self(), name);
}
}// namespace RingSwarm::core