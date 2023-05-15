#ifndef RINGSWARM_BUFFER_H
#define RINGSWARM_BUFFER_H

#include <cstdint>
#include <memory>
#include <vector>
#include "../proto/ProtocolException.h"
#include "../core/Id.h"
#include "../core/ChunkLink.h"
#include "DataSerialisationException.h"
#include <concepts>

namespace RingSwarm::core {
    class Node;
}
namespace RingSwarm::transport {
    class Buffer {
        uint8_t *data;
        uint32_t len;
        uint32_t offset;

        void writeData(const char *data, uint32_t len);

        void readData(char *data, uint32_t len);

    public:

        explicit Buffer(uint32_t len, uint32_t offset = 0);

        explicit Buffer(std::vector<char> initValue);

        /*template<typename... Args>
        Buffer(Args &&... args):Buffer(123) {
            write<Args...>(args...);
        }*/

        ~Buffer();

        uint32_t getWrittenSize() {
            return offset;
        }

        uint8_t *getData() {
            return data;
        }

        std::vector<char> toBlob();

        template<class T>
        void write(T val);

        template<class Integer>
        void write(std::enable_if_t<std::is_unsigned_v<Integer>, Integer> val) {
            if (len - offset < sizeof(Integer)) {
                throw DataSerialisationException();
            }
            *reinterpret_cast<Integer *>(data + offset) = val;
            offset += sizeof(Integer);
        }

        template<class T>
        std::enable_if_t<!std::is_unsigned_v<T>, T> read();

        template<class Integer>
        std::enable_if_t<std::is_unsigned_v<Integer>, Integer> read() {
            if (len - offset < sizeof(Integer)) {
                throw DataSerialisationException();
            }
            Integer retVal = *reinterpret_cast<Integer *>(data + offset);
            offset += sizeof(Integer);
            return retVal;
        }

        void assertFullyUsed() {
            if (offset != len) {
                throw DataSerialisationException();
            }
        }
    };
}

#endif //RINGSWARM_BUFFER_H
