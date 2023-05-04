#include "StorageManager.h"
#include "StorageException.h"
#include <sqlite3.h>
#include <boost/log/trivial.hpp>

namespace RingSwarm::storage {
    ::sqlite3 *dbConnection = nullptr;
    const char *tableCreateStatement =
            "create table if not exists file_meta\n"
            "(\n"
            "    file_id            blob primary key,\n"
            "    author             blob,\n"
            "    creation_timestamp integer,\n"
            "    chunks_count       integer,\n"
            "    chunk_size         integer,\n"
            "    min_swarm_size     integer,\n"
            "    ring_connectivity  integer,\n"
            "    opts               text,\n"
            "    sign               blob\n"
            ");\n"
            "\n"
            "create table if not exists chunk_link\n"
            "(\n"
            "    file_id     blob,\n"
            "    chunk_index integer,\n"
            "    data_hash   blob,\n"
            "    sign        blob,\n"
            "    primary key (file_id, chunk_index)\n"
            ");\n"
            "\n"
            "create table if not exists node\n"
            "(\n"
            "    node_id         blob primary key,\n"
            "    pub_key         blob,\n"
            "    connection_info blob\n"
            ");\n"
            "\n"
            "create table if not exists file_swarm\n"
            "(\n"
            "    file_id     blob,\n"
            "    swarm_index integer,\n"
            "    node_id     blob\n"
            ");\n"
            "\n"
            "create table if not exists chunk_swarm\n"
            "(\n"
            "    file_id     blob,\n"
            "    chunk_index integer,\n"
            "    node_id     blob\n"
            ");\n"
            "\n"
            "create table if not exists settings\n"
            "(\n"
            "    name  text primary key on conflict replace,\n"
            "    value text\n"
            ");";

    void loadStorage(const char *dbPath) {
        int err;
        if ((err = sqlite3_open(dbPath, &dbConnection)) != SQLITE_OK) {
            BOOST_LOG_TRIVIAL(error) << "SQLITE3 db open error: " << sqlite3_errstr(err);
            throw StorageException();
        }

        if ((err = sqlite3_trace_v2(dbConnection, SQLITE_TRACE_STMT,
                                    [](auto mask, void *ctx, void *b, void *c) {
                                        if (mask == SQLITE_TRACE_STMT) {
                                            auto *statement = static_cast<sqlite3_stmt *>(b);
                                            char *ptr = sqlite3_expanded_sql(statement);
                                            std::string str(sqlite3_expanded_sql(statement));
                                            sqlite3_free(ptr);
                                            std::replace(str.begin(), str.end(), '\n', ' ');
                                            BOOST_LOG_TRIVIAL(trace) << "SQLITE3 trace stmt: "
                                                                     << str;
                                        }
                                        return 0;
                                    },
                                    nullptr)) != SQLITE_OK) {
            BOOST_LOG_TRIVIAL(error) << "SQLITE3 add sql tracer error: " << sqlite3_errstr(err);
            throw StorageException();
        }
        if ((err = sqlite3_exec(dbConnection, tableCreateStatement, nullptr, nullptr, nullptr)) != SQLITE_OK) {
            BOOST_LOG_TRIVIAL(error) << "SQLITE3 table create error: " << sqlite3_errstr(err);
            throw StorageException();
        }
    }

    void closeStorage() {
        sqlite3_close(dbConnection);
    }
}