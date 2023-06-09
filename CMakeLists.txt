cmake_minimum_required(VERSION 3.24)
project(RingSwarm LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)

set(OPENSSL_USE_STATIC_LIBS ON)
find_package(OpenSSL REQUIRED COMPONENTS Crypto)

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED system log program_options)

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
        GIT_TAG b5eea8f01de1ce126ba9993390731e820cdcc1bc
        GIT_SHALLOW true
)

FetchContent_MakeAvailable(sqlite3 uvw)
add_library(sqlite3 ${sqlite3_SOURCE_DIR}/sqlite3.c ${sqlite3_SOURCE_DIR}/sqlite3.h ${sqlite3_SOURCE_DIR}/sqlite3ext.h)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")
find_package(fuse3 REQUIRED)

add_executable(RingSwarm
        src/main.cpp
        src/core/Id.cpp
        src/core/Id.h
        src/core/ChunkLink.cpp
        src/core/ChunkLink.h
        src/transport/Buffer.cpp
        src/transport/Buffer.h
        src/transport/Transport.h
        src/proto/ServerHandler.cpp
        src/proto/ServerHandler.h
        src/proto/handler/HandshakeHandler.cpp
        src/proto/ProtocolException.h
        src/transport/PlainSocketTransport.cpp
        src/transport/PlainSocketTransport.h
        src/proto/handler/GetChunkLinkHandler.cpp
        src/storage/ChunkSwarmStorage.cpp
        src/storage/ChunkSwarmStorage.h
        src/core/ChunkSwarm.cpp
        src/core/ChunkSwarm.h
        src/storage/KeySwarmStorage.cpp
        src/storage/KeySwarmStorage.h
        src/core/KeySwarm.h
        src/proto/handler/GetKeyHandler.cpp
        src/core/Node.cpp
        src/core/Node.h
        src/proto/handler/GetNearestChunkHandler.cpp
        src/proto/handler/GetChunkHandler.cpp
        src/proto/handler/DragIntoKeySwarmHandler.cpp
        src/proto/handler/NoticeJoinedKeySwarmHandler.cpp
        src/proto/handler/GetKeySwarmHandler.cpp
        src/proto/handler/GetChunkSwarmHandler.cpp
        src/proto/handler/NoticeJoinedChunkSwarmHandler.cpp
        src/proto/handler/NoticeLeavedChunkSwarmHandler.cpp
        src/proto/handler/SubscribeOnChunkChangeHandler.cpp
        src/proto/handler/ChunkChangeEventHandler.cpp
        src/proto/handler/UnsubscribeOnChunkChangeHandler.cpp
        src/transport/PlainSocketServer.cpp
        src/transport/PlainSocketServer.h
        src/proto/ClientHandler.cpp
        src/proto/ClientHandler.h
        src/proto/ResponseBuffer.h
        src/proto/RequestBuffer.h
        src/proto/TooLargeMessageException.h
        src/core/RingSwarmException.h
        src/transport/TransportBackendException.h
        src/transport/DataSerialisationException.h
        src/proto/ResponseHeader.h
        src/proto/RequestHeader.h
        src/storage/StorageManager.h
        src/storage/StorageManager.h
        src/storage/StorageManager.cpp
        src/storage/StorageException.h
        src/storage/StorageException.h
        src/storage/Statement.cpp
        src/storage/Statement.h
        src/storage/NodeStorage.cpp
        src/storage/NodeStorage.h
        src/storage/ChunkRingStorage.cpp
        src/storage/ChunkRingStorage.h
        src/core/ChunkRing.h
        src/transport/connectionInfo/ConnectionInfo.h
        src/crypto/AsymmetricalCrypto.cpp
        src/crypto/AsymmetricalCrypto.h
        src/crypto/CryptoException.h
        src/crypto/HashCrypto.cpp
        src/crypto/HashCrypto.h
        src/core/Settings.cpp
        src/core/Settings.h
        src/client/FileUploader.cpp
        src/client/FileUploader.h
        src/storage/ClonedEntityException.h
        src/core/ConnectionManager.cpp
        src/core/ConnectionManager.h
        src/transport/connectionInfo/PlainSocketConnectionInfo.cpp
        src/transport/connectionInfo/PlainSocketConnectionInfo.h
        src/transport/connectionInfo/ConnectionInfo.cpp
        src/transport/SecureOverlayTransport.cpp
        src/transport/SecureOverlayTransport.h
        src/crypto/SymmetricalCrypher.cpp
        src/crypto/SymmetricalCrypher.h
        src/client/FileDownloader.cpp
        src/client/FileDownloader.h
        src/client/ExternalKeyHandler.cpp
        src/client/ExternalKeyHandler.h
        src/core/PublicKey.h
        src/core/PublicKey.cpp
        src/crypto/KeyPair.cpp
        src/crypto/KeyPair.h
        src/crypto/Signature.cpp
        src/crypto/Signature.h
        src/storage/ChunkStorage.cpp
        src/storage/ChunkStorage.h
        src/storage/MappedChunk.cpp
        src/storage/MappedChunk.h
        src/fuse/FuseController.cpp
        src/fuse/FuseController.h
        src/fuse/FuseException.h
        src/core/KeyInfo.cpp
        src/core/KeyInfo.h
        src/storage/ChunkIdComparator.h
        src/storage/KeyIndexedStorages.h
        src/client/KeyHandler.h
        src/client/CachedKeyHandler.cpp
        src/client/CachedKeyHandler.h
        src/async/TaskManager.cpp
        src/async/TaskManager.h
        src/async/Future.h
        src/proto/TransportServerSideWrapper.cpp
        src/proto/TransportServerSideWrapper.h src/proto/TransportClientSideWrapper.cpp src/proto/TransportClientSideWrapper.h src/transport/TransportWrapper.h src/async/Spinlock.h src/transport/Transport.cpp src/async/EventLoop.h src/async/EventLoop.cpp)

if (WIN32)
    target_compile_definitions(RingSwarm PRIVATE TGT_WINDOWS)
elseif (UNIX)
    target_compile_definitions(RingSwarm PRIVATE TGT_LINUX)
elseif (APPLE)
    # maybe will support later
    target_compile_definitions(RingSwarm PRIVATE TGT_APPLE)
endif ()

target_compile_definitions(RingSwarm PRIVATE OPENSSL_NO_DEPRECATED)
target_include_directories(RingSwarm PRIVATE ${OpenSSL_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} ${sqlite3_SOURCE_DIR} ${fuse3_INCLUDE_DIRS} ${uvw_SOURCE_DIR}/src)
target_link_libraries(RingSwarm PRIVATE OpenSSL::Crypto ${Boost_LIBRARIES} sqlite3 fuse3::fuse3 uvw::uvw-static uv::uv-static)