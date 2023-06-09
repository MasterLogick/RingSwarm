#ifndef RINGSWARM_TRANSPORTSERVERSIDEWRAPPER_H
#define RINGSWARM_TRANSPORTSERVERSIDEWRAPPER_H

#include "ResponseBuffer.h"
#include "../transport/TransportWrapper.h"
#include <set>
#include <bitset>

namespace RingSwarm::proto {
    class TransportServerSideWrapper : public transport::TransportWrapper {
        uint32_t responseRemainderSize{};
        std::shared_ptr<async::Future<void>> pendingLongResponse;
        std::shared_ptr<async::Future<void>> responseSent;
    public:
        std::bitset<256> activeTags;

        explicit TransportServerSideWrapper(transport::Transport *transport) :
                transport::TransportWrapper(transport), activeTags() {}

        void sendResponse(ResponseBuffer &resp, uint8_t responseType, uint8_t tag);

        void startLongResponse(uint32_t size, uint8_t responseType, uint8_t tag);

        inline void sendError(uint8_t tag) {
            sendEmptyResponse(0, tag);
        }

        void sendEmptyResponse(uint8_t responseType, uint8_t tag) {
            ResponseBuffer b(0);
            sendResponse(b, responseType, tag);
        }

        void rawWrite(void *data, uint32_t len) override;
    };
}

#endif //RINGSWARM_TRANSPORTSERVERSIDEWRAPPER_H
