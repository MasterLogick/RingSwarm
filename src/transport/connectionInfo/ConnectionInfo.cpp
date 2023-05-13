#include "ConnectionInfo.h"
#include "PlainSocketConnectionInfo.h"
#include "../DataSerialisationException.h"

namespace RingSwarm::transport {
    typedef ConnectionInfo *(*Parser)(Buffer &buffer);

    constexpr Parser Parsers[] = {
            nullptr,
            &PlainSocketConnectionInfo::parseConnectionInfo
    };
    constexpr uint16_t ParsersCount = 2;
    static_assert(sizeof(Parsers) == sizeof(Parser) * ParsersCount);

    ConnectionInfo *ConnectionInfo::parseConnectionInfo(Buffer &buffer) {
        uint8_t type = buffer.readUint8();
        if (type >= ParsersCount) {
            throw DataSerialisationException();
        }
        return Parsers[type](buffer);
    }
}

