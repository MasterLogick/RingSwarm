#ifndef RINGSWARM_RINGSWARMEXCEPTION_H
#define RINGSWARM_RINGSWARMEXCEPTION_H

#include <boost/stacktrace.hpp>
#include <exception>
#include <iostream>

namespace RingSwarm::core {
class RingSwarmException : public std::exception {
    std::string reason;

public:
    RingSwarmException() = default;

    explicit RingSwarmException(std::string reason) : reason(std::move(reason)) {
    }

    [[nodiscard]] const char *what() const noexcept override {
        return reason.c_str();
    }
};
}// namespace RingSwarm::core
#endif//RINGSWARM_RINGSWARMEXCEPTION_H
