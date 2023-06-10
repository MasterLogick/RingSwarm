#ifndef RINGSWARM_BUFFER_H
#define RINGSWARM_BUFFER_H

#include <cstdint>
#include <type_traits>
#include <memory>
#include <vector>
#include "../proto/ProtocolException.h"
#include "../core/Id.h"
#include "../core/ChunkLink.h"
#include "DataSerialisationException.h"
#include <concepts>
#include <map>

namespace RingSwarm::core {
    class Node;
}
namespace RingSwarm::transport {
    class Buffer {
        void writeData(const char *data, uint32_t len);

        void readData(char *data, uint32_t len);

        template<class T>
        static constexpr uint32_t calcSize(T) requires std::unsigned_integral<T> {
            return sizeof(T);
        }

        static constexpr uint32_t calcSize(core::Id *) {
            return 32;
        }

        static constexpr uint32_t calcSize(crypto::Signature *) {
            return 72;
        }

        static constexpr uint32_t calcSize(core::PublicKey *) {
            return 33;
        }

        static uint32_t calcSize(core::Node *n);

        static constexpr uint32_t calcSize(core::ChunkLink *) {
            return 2 * calcSize((core::Id *) nullptr) + calcSize<uint64_t>(0) + calcSize((crypto::Signature *) nullptr);
        }

        static uint32_t calcSize(std::string &s) {
            return calcSize<uint32_t>(0) + s.size();
        }

        template<class T, class ...Args>
        static uint32_t calcSize(T val, Args ...args) {
            return calcSize(val) + calcSize(args...);
        }

    public:

        Buffer() : data(nullptr), offset(0), len(0) {}

        Buffer(const Buffer &buffer);

        Buffer(Buffer &&buffer);

        Buffer &operator=(const Buffer &buffer) = default;

        explicit Buffer(uint32_t len, uint32_t offset = 0);

        explicit Buffer(uint8_t *ptr, uint32_t len) : data(ptr), offset(0), len(len) {}

        explicit Buffer(std::vector<char> initValue);

        ~Buffer();

        uint8_t *data;
        uint32_t len;
        uint32_t offset;

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
        void write(std::vector<T> &vec) {
            write<uint32_t>(vec.size());
            for (const auto &item: vec) {
                write<T>(item);
            }
        }

        template<class T, class V>
        void write(std::map<T, V> &map) {
            write<uint32_t>(map.size());
            for (const auto &item: map) {
                write<T>(item.first);
                write<V>(item.second);
            }
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

        template<class T>
        std::vector<T> readVec() {
            uint32_t size = read<uint32_t>();
            std::vector<T> vec;
            vec.reserve(size);
            for (int i = 0; i < size; ++i) {
                vec.push_back(read<T>());
            }
            return vec;
        }

        template<class T, class V>
        std::map<T, V> readMap() {
            uint32_t size = read<uint32_t>();
            std::map<T, V> map;
            for (int i = 0; i < size; ++i) {
                auto key = read<T>();
                auto val = read<V>();
                map[key] = val;
            }
            return map;
        }


        void assertFullyUsed() {
            if (offset != len) {
                throw DataSerialisationException();
            }
        }
    };
}

#endif //RINGSWARM_BUFFER_H
