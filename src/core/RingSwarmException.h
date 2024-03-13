#ifndef RINGSWARM_RINGSWARMEXCEPTION_H
#define RINGSWARM_RINGSWARMEXCEPTION_H

#include <boost/stacktrace.hpp>
#include <exception>
#include <iostream>

namespace RingSwarm::core {
class RingSwarmException : public std::exception {
public:
    RingSwarmException() {
        std::cout << boost::stacktrace::stacktrace();
    }
};
}// namespace RingSwarm::core

#endif//RINGSWARM_RINGSWARMEXCEPTION_H
