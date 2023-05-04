#include "Statement.h"
#include "StorageException.h"
#include <sqlite3.h>
#include <boost/log/trivial.hpp>

namespace RingSwarm::storage {
    Statement::Statement(sqlite3 *connection, const char *sql) {
        if ((sqlite3_prepare_v2(connection, sql, -1, &statement, nullptr)) != SQLITE_OK) {

            BOOST_LOG_TRIVIAL(error) << "SQLITE3 \"" << sql << "\" prepare error: "
                                     << sqlite3_errmsg(sqlite3_db_handle(statement));
            throw StorageException();
        }
    }

    Statement::~Statement() {
        sqlite3_finalize(statement);
    }

    void Statement::bindText(const char *label, std::string value) {
        int index = sqlite3_bind_parameter_index(statement, label);
        if (index == 0) {
            BOOST_LOG_TRIVIAL(error) << "SQLITE3 wrong index: " << label;
            throw StorageException();
        }
        if ((sqlite3_bind_text(statement, index, value.c_str(), value.length(), SQLITE_TRANSIENT)) != SQLITE_OK) {
            BOOST_LOG_TRIVIAL(error) << "SQLITE3 bind \"" << label << "\"=\"" << value << "\": "
                                     << sqlite3_errmsg(sqlite3_db_handle(statement));
            throw StorageException();
        }
    }

    void Statement::bindInt64(const char *label, uint64_t value) {
        int index = sqlite3_bind_parameter_index(statement, label);
        if (index == 0) {
            BOOST_LOG_TRIVIAL(error) << "SQLITE3 wrong index: " << label;
            throw StorageException();
        }
        if ((sqlite3_bind_int64(statement, index, value)) != SQLITE_OK) {
            BOOST_LOG_TRIVIAL(error) << "SQLITE3 bind \"" << label << "\"=\"" << value << "\": "
                                     << sqlite3_errmsg(sqlite3_db_handle(statement));
            throw StorageException();
        }
    }

    void Statement::bindInt32(const char *label, uint32_t value) {
        int index = sqlite3_bind_parameter_index(statement, label);
        if (index == 0) {
            BOOST_LOG_TRIVIAL(error) << "SQLITE3 wrong index: " << label;
            throw StorageException();
        }
        if ((sqlite3_bind_int(statement, index, value)) != SQLITE_OK) {
            BOOST_LOG_TRIVIAL(error) << "SQLITE3 bind \"" << label << "\"=\"" << value << "\": "
                                     << sqlite3_errmsg(sqlite3_db_handle(statement));
            throw StorageException();
        }
    }

    void Statement::bindBlob(const char *label, const void *blob, int size) {
        int index = sqlite3_bind_parameter_index(statement, label);
        if (index == 0) {
            BOOST_LOG_TRIVIAL(error) << "SQLITE3 wrong index: " << label;
            throw StorageException();
        }
        if ((sqlite3_bind_blob(statement, index, blob, size, SQLITE_TRANSIENT)) != SQLITE_OK) {
            BOOST_LOG_TRIVIAL(error) << "SQLITE3 bind blob \"" << label << "\": "
                                     << sqlite3_errmsg(sqlite3_db_handle(statement));
            throw StorageException();
        }
    }

    bool Statement::nextRow() {
        int err;
        if ((err = sqlite3_step(statement)) == SQLITE_ROW) {
            return true;
        } else if (err == SQLITE_DONE) {
            return false;
        } else {
            BOOST_LOG_TRIVIAL(error) << "SQLITE3 step error: " << sqlite3_errmsg(sqlite3_db_handle(statement));
            throw StorageException();
        }
    }

    std::string Statement::getText(int n) {
        return {reinterpret_cast<const char *>(sqlite3_column_text(statement, n))};
    }

    uint32_t Statement::getInt32(int n) {
        return sqlite3_column_int(statement, n);
    }

    uint64_t Statement::getInt64(int n) {
        return sqlite3_column_int64(statement, n);
    }

    std::vector<char> Statement::getBlob(int n) {
        auto *ptr = reinterpret_cast<const char *>(sqlite3_column_blob(statement, n));
        int size = sqlite3_column_bytes(statement, n);
        return {ptr, ptr + size};
    }

    void Statement::reset() {
        sqlite3_reset(statement);
    }
} // storage