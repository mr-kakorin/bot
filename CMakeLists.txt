cmake_minimum_required(VERSION 3.7)
project(mybot)

if (NOT DEFINED REP_ROOT)
    set(REP_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
endif (NOT DEFINED REP_ROOT)

set(CMAKE_FIND_LIBRARY_SUFFIXES .a ${CMAKE_FIND_LIBRARY_SUFFIXES})

set(CMAKE_CXX_STANDARD 17)

set(Boost_USE_MULTITHREADED ON)

set(CMAKE_BUILD_TYPE "Debug")

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    message("debug build")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -DDEBUG -D_DEBUG -ggdb3")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -DDEBUG -D_DEBUG -ggdb3")
else ()
    message("release build")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -march=native -ffast-math -flto")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native -ffast-math -flto")
endif ()


find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Boost COMPONENTS system REQUIRED)
find_package(CURL)
include_directories(/usr/local/include ${OPENSSL_INCLUDE_DIR} ${Boost_INCLUDE_DIR} ${REP_ROOT}/SQLiteCpp/include)
if (CURL_FOUND)
    include_directories(${CURL_INCLUDE_DIRS})
    add_definitions(-DHAVE_CURL)
endif ()

add_subdirectory(tgbot-cpp)
add_subdirectory(SQLiteCpp)

include_directories(src)

set(sourcedir src)
set(sourcefiles ${sourcedir}/*.cpp)
set(headerfiles ${sourcedir}/*.hpp)
file(GLOB_RECURSE sources ${sourcefiles})
file(GLOB_RECURSE headers ${headerfiles})

add_executable(${PROJECT_NAME} ${sources} ${headers})
message("${CMAKE_THREAD_LIBS_INIT} ${OPENSSL_LIBRARIES} ${Boost_LIBRARIES} ${CURL_LIBRARIES}")
#target_link_libraries(${PROJECT_NAME} TgBot ${CMAKE_THREAD_LIBS_INIT} ${OPENSSL_LIBRARIES} ${Boost_LIBRARIES} ${CURL_LIBRARIES})

add_dependencies(${PROJECT_NAME} SQLiteCpp)
add_dependencies(${PROJECT_NAME} sqlite3)
add_dependencies(${PROJECT_NAME} TgBot)

target_link_libraries(${PROJECT_NAME}
        -static
        TgBot
        ${CMAKE_THREAD_LIBS_INIT} ${OPENSSL_LIBRARIES} ${Boost_LIBRARIES} ${CURL_LIBRARIES}
        ${CMAKE_BINARY_DIR}/SQLiteCpp/libSQLiteCpp.a
        ${CMAKE_BINARY_DIR}/SQLiteCpp/sqlite3/libsqlite3.a
        pthread dl)