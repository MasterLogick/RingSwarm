#ifndef RINGSWARM_RINGSWARMEXCEPTION_H
#define RINGSWARM_RINGSWARMEXCEPTION_H

#include <exception>
#include <boost/stacktrace.hpp>
#include <iostream>

namespace RingSwarm::core {
    class RingSwarmException : public std::exception {
    public:
        RingSwarmException() {
            std::cout << boost::stacktrace::stacktrace();
        }
    };
}

#endif //RINGSWARM_RINGSWARMEXCEPTION_H
