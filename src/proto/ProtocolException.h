#ifndef RINGSWARM_PROTOCOLEXCEPTION_H
#define RINGSWARM_PROTOCOLEXCEPTION_H

#include <exception>

namespace RingSwarm::proto {
    class ProtocolException : public std::exception {
    };
}

#endif //RINGSWARM_PROTOCOLEXCEPTION_H
