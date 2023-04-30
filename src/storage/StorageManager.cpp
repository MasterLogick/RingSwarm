#include "StorageManager.h"
#include "StorageException.h"
#include <sqlite3.h>
#include <boost/log/trivial.hpp>

namespace RingSwarm::storage {
    ::sqlite3 *dbConnection = nullptr;
    const char *tableCreateStatement =
            "create table if not exists file_meta\n"
            "(\n"
            "    id                 integer primary key autoincrement,\n"
            "    file_id            blob unique,\n"
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
            "    id          integer primary key autoincrement,\n"
            "    file_id     blob,\n"
            "    chunk_index integer,\n"
            "    data_hash   blob,\n"
            "    sign        blob\n"
            ");\n"
            "\n"
            "create table if not exists node\n"
            "(\n"
            "    id              integer primary key autoincrement,\n"
            "    user_id         blob,\n"
            "    connection_info blob\n"
            ");\n"
            "\n"
            "create table if not exists file_swarm\n"
            "(\n"
            "    file_meta_id integer,\n"
            "    swarm_index  integer,\n"
            "    node_id      integer,\n"
            "    foreign key (file_meta_id) references file_meta (id),\n"
            "    foreign key (node_id) references node (id)\n"
            ");\n"
            "\n"
            "create table if not exists chunk_swarm\n"
            "(\n"
            "    chunk_id integer,\n"
            "    node_id  integer,\n"
            "    foreign key (chunk_id) references chunk_link (id),\n"
            "    foreign key (node_id) references node (id)\n"
            ");";

    void loadStorage(const char *dbPath) {
        int err;
        if ((err = sqlite3_open(dbPath, &dbConnection)) != SQLITE_OK) {
            BOOST_LOG_TRIVIAL(error) << "SQLITE3 db open error: " << sqlite3_errstr(err);
            throw StorageException();
        }

        if ((err = sqlite3_exec(dbConnection, tableCreateStatement, nullptr, nullptr, nullptr)) != SQLITE_OK) {
            BOOST_LOG_TRIVIAL(error) << "SQLITE3 table create error: " << sqlite3_errstr(err);
            throw StorageException();
        }
        sqlite3_trace_v2(dbConnection, SQLITE_TRACE_STMT,
                         [](auto mask, void *ctx, void *b, void *c) {
                             if (mask == SQLITE_TRACE_STMT) {
                                 auto *statement = static_cast<sqlite3_stmt *>(b);
                                 BOOST_LOG_TRIVIAL(trace) << "SQLITE3 trace stmt: " << sqlite3_expanded_sql(statement);
                             }
                             return 0;
                         },
                         nullptr);
    }

    void closeStorage() {
        sqlite3_close(dbConnection);
    }
}