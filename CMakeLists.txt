# cmake_minimum_required(VERSION 3.10)

# project(apollocpp)

# set(CMAKE_VERBOSE_MAKEFILE ON)
# set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -O0 -g -std=c++17 -Wall -Wno-deprecated -Werror -Wno-unused-function")
# set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
# set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

# include_directories(${PROJECT_SOURCE_DIR}/include)

# file(GLOB ApolloConfig "${PROJECT_SOURCE_DIR}/src/*.cpp")

# find_package(nlohmann_json 3.2.0 REQUIRED)
# find_package(CURL REQUIRED)
# find_library(LOG4CPP_LIB log4cpp) # 将其打包进去
# find_package(Threads REQUIRED)
# find_package(OpenSSL REQUIRED)


# # 由于这个第三方库需要外部的库，所以将外部的工具库链接到这里即可
# add_library(Apollocppclient SHARED ${ApolloConfig})
# target_link_libraries(Apollocppclient PRIVATE ${CURL_LIBRARIES} ${LOG4CPP_LIB} ${CURL_LIBRARIES} nlohmann_json::nlohmann_json Threads::Threads OpenSSL::SSL OpenSSL::Crypto)  # 添加这行

# add_executable(test ./tests/ApolloConfigTest.cpp)
# add_dependencies(test Apollocppclient)
# target_link_libraries(test PRIVATE Apollocppclient  ${LOG4CPP_LIB})

cmake_minimum_required(VERSION 3.10)

project(apollocpp)

# Enable detailed build output
set(CMAKE_VERBOSE_MAKEFILE ON)

# Set compiler flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g -Wall -Werror -Wno-deprecated -Wno-unused-function")

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)

# Source files
file(GLOB_RECURSE APOLLO_SRC "${PROJECT_SOURCE_DIR}/src/*.cpp")

# Find required packages
find_package(nlohmann_json 3.2.0 REQUIRED)
find_package(CURL REQUIRED)
find_library(LOG4CPP_LIB log4cpp REQUIRED)
find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)

# Add shared library target
add_library(apollocpp_client SHARED ${APOLLO_SRC})

# Link libraries to the shared library target
target_link_libraries(apollocpp_client PRIVATE 
    CURL::libcurl 
    ${LOG4CPP_LIB} 
    nlohmann_json::nlohmann_json 
    Threads::Threads 
    OpenSSL::SSL 
    OpenSSL::Crypto)

# Add test executable
add_executable(test_apollocpp ./tests/ApolloConfigTest.cpp)

# Set dependencies and link libraries for the test executable
add_dependencies(test_apollocpp apollocpp_client)
target_link_libraries(test_apollocpp PRIVATE apollocpp_client ${LOG4CPP_LIB})

# Installation settings
install(TARGETS apollocpp_client test_apollocpp
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/ DESTINATION include)
