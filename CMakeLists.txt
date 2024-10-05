cmake_minimum_required(VERSION 3.10)

project(apollocpp)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -O0 -g -std=c++17 -Wall -Wno-deprecated -Werror -Wno-unused-function")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

include_directories(${PROJECT_SOURCE_DIR}/include)

file(GLOB ApolloConfig "${PROJECT_SOURCE_DIR}/src/*.cpp")

find_package(nlohmann_json 3.2.0 REQUIRED)
find_package(CURL REQUIRED)
find_library(LOG4CPP_LIB log4cpp) # 将其打包进去
find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)


# 由于这个第三方库需要外部的库，所以将外部的工具库链接到这里即可
add_library(Apollocppclient SHARED ${ApolloConfig})
target_link_libraries(Apollocppclient PRIVATE ${CURL_LIBRARIES} ${LOG4CPP_LIB} ${CURL_LIBRARIES} nlohmann_json::nlohmann_json Threads::Threads OpenSSL::SSL OpenSSL::Crypto)  # 添加这行

add_executable(test ./tests/ApolloConfigTest.cpp)
add_dependencies(test Apollocppclient)
target_link_libraries(test PRIVATE Apollocppclient  ${LOG4CPP_LIB})
