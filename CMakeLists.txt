cmake_minimum_required(VERSION 3.24)
project(RingSwarm)

set(CMAKE_CXX_STANDARD 20)

find_package(OpenSSL REQUIRED COMPONENTS Crypto)
find_library(gmp NAMES gmp)
find_library(gmpxx NAMES gmpxx)

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED system log)

set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")

add_library(sqlite3 sqlite3/sqlite3.c sqlite3/sqlite3.h)

add_executable(RingSwarm src/main.cpp src/core/FileMeta.cpp src/core/FileMeta.h src/core/Id.cpp src/core/Id.h src/core/ChunkLink.cpp src/core/ChunkLink.h src/transport/Buffer.cpp src/transport/Buffer.h src/transport/Transport.cpp src/transport/Transport.h src/proto/ServerHandler.cpp src/proto/ServerHandler.h src/proto/handler/HandshakeHandler.cpp src/proto/ProtocolException.h src/transport/PlainSocketTransport.cpp src/transport/PlainSocketTransport.h src/proto/handler/GetChunkLinkHandler.cpp src/storage/ChunkSwarmStorage.cpp src/storage/ChunkSwarmStorage.h src/core/ChunkSwarm.cpp src/core/ChunkSwarm.h src/storage/FileSwarmStorage.cpp src/storage/FileSwarmStorage.h src/core/FileMetaSwarm.cpp src/core/FileMetaSwarm.h src/proto/handler/GetFileMetaHandler.cpp src/storage/ConnectionsStorage.cpp src/storage/ConnectionsStorage.h src/core/Node.cpp src/core/Node.h src/proto/handler/GetNearestChunkHandler.cpp src/core/Random.cpp src/core/Random.h src/proto/handler/GetChunkHandler.cpp src/proto/handler/DragIntoFileMetaSwarmHandler.cpp src/proto/handler/NoticeJoinedFileMetaSwarmHandler.cpp src/proto/handler/GetFileMetaSwarmHandler.cpp src/proto/handler/GetChunkSwarmHandler.cpp src/proto/handler/NoticeJoinedChunkSwarmHandler.cpp src/proto/handler/NoticeLeavedChunkSwarmHandler.cpp src/proto/handler/SubscribeOnChunkChangeHandler.cpp src/proto/handler/ChunkChangeEventHandler.cpp src/proto/handler/UnsubscribeOnChunkChangeHandler.cpp src/transport/PlainSocketServer.cpp src/transport/PlainSocketServer.h src/proto/ClientHandler.cpp src/proto/ClientHandler.h src/transport/ResponseBuffer.h src/transport/RequestBuffer.h src/proto/TooLargeMessageException.h src/core/RingSwarmException.h src/transport/TransportBackendException.h src/transport/DataSerialisationException.h src/proto/ResponseHeader.h src/transport/ResponseBuffer.cpp src/proto/RequestHeader.h src/storage/StorageManager.h src/storage/StorageManager.h src/storage/StorageManager.cpp src/storage/StorageException.h src/storage/StorageException.h src/storage/Statement.cpp src/storage/Statement.h src/storage/NodeStorage.cpp src/storage/NodeStorage.h src/storage/ChunkRingStorage.cpp src/storage/ChunkRingStorage.h src/core/Ring.cpp src/core/ChunkRing.h src/core/ConnectionInfo.cpp src/core/ConnectionInfo.h src/crypto/AsymetricalCrypto.cpp src/crypto/AsymetricalCrypto.h src/crypto/CryptoException.h src/crypto/HashCrypto.cpp src/crypto/HashCrypto.h src/core/Settings.cpp src/core/Settings.h)

set_target_properties(RingSwarm PROPERTIES LINK_SEARCH_START_STATIC 1)
set_target_properties(RingSwarm PROPERTIES LINK_SEARCH_END_STATIC 1)

target_compile_definitions(RingSwarm PUBLIC OPENSSL_NO_DEPRECATED)
target_include_directories(RingSwarm PUBLIC "${OPENSSL_INCLUDE_DIR}" sqlite3)
target_link_libraries(RingSwarm PUBLIC OpenSSL::Crypto gmp gmpxx ${Boost_LIBRARIES} sqlite3)