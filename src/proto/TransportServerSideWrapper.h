#ifndef RINGSWARM_TRANSPORTSERVERSIDEWRAPPER_H
#define RINGSWARM_TRANSPORTSERVERSIDEWRAPPER_H

#include "ResponseBuffer.h"
#include "../transport/TransportWrapper.h"
#include "LongResponseTransport.h"
#include <set>
#include <bitset>

namespace RingSwarm::proto {
    class TransportServerSideWrapper : public transport::TransportWrapper {
        async::Spinlock lock;
        std::shared_ptr<async::Future<void>> responseSent;
    public:
        std::bitset<256> activeTags;

        explicit TransportServerSideWrapper(transport::Transport *transport) :
                transport::TransportWrapper(transport), activeTags(),
                responseSent(async::Future<void>::createResolved()) {}

        void scheduleResponse(std::shared_ptr<ResponseBuffer> resp, uint8_t responseType, uint8_t tag);

        std::shared_ptr<async::Future<LongResponseTransport *>> scheduleLongResponse(
                uint32_t size,
                uint8_t responseType,
                uint8_t tag
        );

        inline void sendError(uint8_t tag) {
            sendEmptyResponse(0, tag);
        }

        void sendEmptyResponse(uint8_t responseType, uint8_t tag) {
            scheduleResponse(std::make_shared<ResponseBuffer>(0), responseType, tag);
        }

        void rawWrite(void *data, uint32_t len) override;
    };
}

#endif //RINGSWARM_TRANSPORTSERVERSIDEWRAPPER_H
