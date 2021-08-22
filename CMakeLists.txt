cmake_minimum_required(VERSION 3.20)
project(sockets CXX)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)

FetchContent_Declare(
        glog
        GIT_REPOSITORY https://github.com/google/glog.git
        GIT_TAG v0.5.0
)

FetchContent_MakeAvailable(glog)

FetchContent_GetProperties(glog)
if (NOT glog_POPULATED)
    FetchContent_Populate(glog)
    add_subdirectory(${glog_SOURCE_DIR} ${glog_BINARY_DIR})
endif()

set(PARSER_FILES parser.h parser.cpp)
set(SERIALIZATION_FILES serializer.h serializer.cpp)
set(DESERIALIZATION_FILES deserializer.h deserializer.cpp)
set(CLIENT_FILES client/client.h client/client.cpp)
set(SERVER_FILES server/server.h server/server.cpp)
set(READ_WRITE_FILES read_write.h read_write.cpp)

add_executable(client client/main.cpp ${CLIENT_FILES} ${PARSER_FILES} ${DESERIALIZATION_FILES} ${READ_WRITE_FILES})
target_link_libraries(client glog::glog)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(server server/main.cpp ${SERVER_FILES} ${PARSER_FILES} ${SERIALIZATION_FILES} ${READ_WRITE_FILES})
target_link_libraries(server Threads::Threads glog::glog)