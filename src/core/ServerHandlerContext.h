#ifndef RINGSWARM_SRC_CORE_SERVERHANDLERCONTEXT_H
#define RINGSWARM_SRC_CORE_SERVERHANDLERCONTEXT_H

#include "../async/coroutine.h"
#include "../transport/Transport.h"

namespace RingSwarm::core {
struct ServerHandlerContext {
    async::Coroutine<> coroutine;
    std::shared_ptr<transport::Transport> serverSideTransport;
};
} // namespace RingSwarm::core

#endif // RINGSWARM_SRC_CORE_SERVERHANDLERCONTEXT_H
