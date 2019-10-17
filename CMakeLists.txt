cmake_minimum_required(VERSION 3.8)
project(mybot)

set(CMAKE_CXX_STANDARD 17)

set(Boost_USE_MULTITHREADED ON)

set(CMAKE_BUILD_TYPE "Debug")

find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Boost COMPONENTS system REQUIRED)
find_package(CURL)
include_directories(/usr/local/include ${OPENSSL_INCLUDE_DIR} ${Boost_INCLUDE_DIR})
if (CURL_FOUND)
    include_directories(${CURL_INCLUDE_DIRS})
    add_definitions(-DHAVE_CURL)
endif ()

add_subdirectory(tgbot-cpp)

set(sourcedir src)
set(sourcefiles ${sourcedir}/*.cpp)
set(headerfiles ${sourcedir}/*.h)
file(GLOB_RECURSE sources ${sourcefiles})
file(GLOB_RECURSE headers ${headerfiles})

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    message("debug build")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -DDEBUG -D_DEBUG -ggdb3")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -DDEBUG -D_DEBUG -ggdb3")
else ()
    message("release build")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3 -march=native -ffast-math -flto")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native -ffast-math -flto")
endif ()

add_executable(${PROJECT_NAME} ${sources} ${headers})

target_link_libraries(${PROJECT_NAME} TgBot ${CMAKE_THREAD_LIBS_INIT} ${OPENSSL_LIBRARIES} ${Boost_LIBRARIES} ${CURL_LIBRARIES})
