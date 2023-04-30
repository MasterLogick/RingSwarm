#ifndef RINGSWARM_STATEMENT_H
#define RINGSWARM_STATEMENT_H

#include <cstdint>
#include <string>
#include <vector>
#include "../core/Id.h"

class sqlite3_stmt;

class sqlite3;
namespace RingSwarm::storage {
    class Statement {
        ::sqlite3_stmt *statement;
    public:
        Statement(sqlite3 *connection, const char *sql);

        void bindText(const char *label, std::string value);

        void bindInt32(const char *label, uint32_t value);

        void bindInt64(const char *label, uint64_t value);

        void bindBlob(const char *label, const void *blob, int size);

        void bindBlob(const char *label, std::vector<char> blob) {
            bindBlob(label, blob.data(), blob.size());
        }

        void bindId(const char *label, RingSwarm::core::Id *id) {
            bindBlob(label, id->hash, 32);
        }

        bool nextRow();

        const char *getText(int n);

        uint64_t getInt64(int n);

        uint32_t getInt32(int n);

        std::vector<char> getBlob(int n);

        core::Id *getId(int n) {
            return core::Id::fromBlob(getBlob(n));
        }

        ~Statement();

        void reset();
    };
}

#endif //RINGSWARM_STATEMENT_H
