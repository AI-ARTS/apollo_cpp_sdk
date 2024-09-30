cmake_minimum_required(VERSION 3.10)

project(apollocpp)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -O0 -g -std=c++17 -Wall -Wno-deprecated -Werror -Wno-unused-function")
# 设置输出路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

# 头文件包含
include_directories(
    ${PROJECT_SOURCE_DIR}/include    
)
file(GLOB ApolloConfig
    "${PROJECT_SOURCE_DIR}/src/ApolloConfig.cpp"
)

file(GLOB HttpClient
    "${PROJECT_SOURCE_DIR}/src/HttpClient.cpp"
)

find_package(nlohmann_json 3.2.0 REQUIRED)
find_package(CURL REQUIRED)

add_library(APOLLOCONFIG SHARED ${ApolloConfig})
add_library(HTTPCLIENT SHARED ${HttpClient})

add_executable(test ./tests/ApolloConfigTest.cpp)
add_dependencies(test APOLLOCONFIG HTTPCLIENT)
target_link_libraries(test PRIVATE APOLLOCONFIG HTTPCLIENT)

# 第三方库函数的绑定
target_link_libraries(test PRIVATE CURL::libcurl nlohmann_json::nlohmann_json)