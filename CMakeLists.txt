cmake_minimum_required(VERSION 3.24)
project(RingSwarm LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)

set(OPENSSL_USE_STATIC_LIBS ON)
find_package(OpenSSL REQUIRED COMPONENTS Crypto)

include(FetchContent)
FetchContent_Declare(
        sqlite3
        URL https://sqlite.org/2023/sqlite-amalgamation-3410200.zip
        URL_HASH SHA3_256=c51ca72411b8453c64e0980be23bc9b9530bdc3ec1513e06fbf022ed0fd02463
        OVERRIDE_FIND_PACKAGE
        TLS_VERIFY true
)

set(USE_LIBCPP OFF)
set(BUILD_UVW_LIBS ON)
FetchContent_Declare(
        uvw
        GIT_REPOSITORY https://github.com/MasterLogick/uvw.git
        GIT_TAG 3a7d1611d8d29cd41148f2b3e14a71028ce0e6ea
        GIT_SHALLOW true
)

set(WITH_GTEST OFF)
set(WITH_GFLAGS OFF)
set(WITH_UNWIND none)
FetchContent_Declare(
        glog
        GIT_REPOSITORY https://github.com/google/glog.git
        GIT_TAG 34b8da6496aec6a98277808701cfa834fae9801f
        GIT_SHALLOW true
)

FetchContent_MakeAvailable(sqlite3 uvw glog)
add_library(sqlite3 ${sqlite3_SOURCE_DIR}/sqlite3.c ${sqlite3_SOURCE_DIR}/sqlite3.h ${sqlite3_SOURCE_DIR}/sqlite3ext.h)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
find_package(fuse3 REQUIRED)

add_executable(RingSwarm
        src/Assert.h
        src/Assert.h
        src/async/ChainedAwaitObject.h
        src/async/Coroutine.h
        src/async/EventLoop.cpp
        src/async/EventLoop.h
        src/async/FinalAwait.h
        src/async/GetCoroutineHandleAwaitObject.h
        src/async/Promise.h
        src/async/SuspendThisCoroutineAwaitObject.h
        src/async/ThreadPool.cpp
        src/async/ThreadPool.h
        src/async/coroutine.h
        src/core/Id.cpp
        src/core/Id.h
        src/core/Node.cpp
        src/core/Node.h
        src/core/PublicKey.cpp
        src/core/PublicKey.h
        src/core/RingSwarmException.h
        src/core/Thread.cpp
        src/core/Thread.h
        src/core/Thread.h
        src/crypto/CryptoException.h
        src/crypto/HashCrypto.cpp
        src/crypto/HashCrypto.h
        src/crypto/KeyPair.cpp
        src/crypto/KeyPair.h
        src/crypto/Signature.cpp
        src/crypto/Signature.h
        src/crypto/SymmetricalCypher.cpp
        src/crypto/SymmetricalCypher.h
        src/main.cpp
        src/proto/ClientHandler.cpp
        src/proto/ClientHandler.h
        src/proto/ClientRequestState.cpp
        src/proto/ClientRequestState.h
        src/proto/ClientTransport.cpp
        src/proto/ClientTransport.h
        src/proto/CommandId.h
        src/proto/ProtocolException.h
        src/proto/RequestBuffer.h
        src/proto/RequestHeader.h
        src/proto/ResponseBuffer.h
        src/proto/ResponseHeader.h
        src/proto/ServerHandler.cpp
        src/proto/ServerHandler.h
        src/proto/ServerResponseState.h
        src/proto/TooLargeMessageException.h
        src/transport/Buffer.cpp
        src/transport/Buffer.h
        src/transport/DataSerialisationException.h
        src/transport/PlainSocketServer.cpp
        src/transport/PlainSocketServer.h
        src/transport/PlainSocketTransport.cpp
        src/transport/PlainSocketTransport.h
        src/transport/SecureOverlayTransport.cpp
        src/transport/SecureOverlayTransport.h
        src/transport/Transport.cpp
        src/transport/Transport.h
        src/transport/TransportWrapper.h
        src/async/EventLoopLock.h
        src/async/EventLoopLock.cpp
        src/core/NodeContext.cpp
        src/core/NodeContext.h
        src/transport/TransportException.h
        src/transport/ClosedTransportException.h
        src/async/AwaitResumeResult.h
        src/proto/shadow/PingHandler.cpp
        src/proto/shadow/GetNodeHandler.cpp
        src/proto/UnknownResponseException.h
        src/async/ResumeSuspendAwaitObject.h
        src/async/SyncCoroutineWaiter.h
        src/core/ServerHandlerContext.h
)

if (WIN32)
    target_compile_definitions(RingSwarm PRIVATE TGT_WINDOWS)
elseif (UNIX)
    target_compile_definitions(RingSwarm PRIVATE TGT_LINUX)
elseif (APPLE)
    # maybe will support later
    target_compile_definitions(RingSwarm PRIVATE TGT_APPLE)
endif ()

target_compile_definitions(RingSwarm PRIVATE OPENSSL_NO_DEPRECATED)
target_include_directories(RingSwarm PRIVATE
        ${OpenSSL_INCLUDE_DIRS}
        ${sqlite3_SOURCE_DIR}
        ${fuse3_INCLUDE_DIRS}
        ${uvw_SOURCE_DIR}/src)
target_link_libraries(RingSwarm PRIVATE
        OpenSSL::Crypto
        sqlite3
        fuse3::fuse3
        uvw::uvw-static
        uv::uv-static
        glog::glog)