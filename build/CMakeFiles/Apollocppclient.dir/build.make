# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Produce verbose output by default.
VERBOSE = 1

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/aldd/JD/apollo_cpp_sdk

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/aldd/JD/apollo_cpp_sdk/build

# Include any dependencies generated for this target.
include CMakeFiles/Apollocppclient.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/Apollocppclient.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/Apollocppclient.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/Apollocppclient.dir/flags.make

CMakeFiles/Apollocppclient.dir/src/ApolloConfig.cpp.o: CMakeFiles/Apollocppclient.dir/flags.make
CMakeFiles/Apollocppclient.dir/src/ApolloConfig.cpp.o: ../src/ApolloConfig.cpp
CMakeFiles/Apollocppclient.dir/src/ApolloConfig.cpp.o: CMakeFiles/Apollocppclient.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/aldd/JD/apollo_cpp_sdk/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/Apollocppclient.dir/src/ApolloConfig.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/Apollocppclient.dir/src/ApolloConfig.cpp.o -MF CMakeFiles/Apollocppclient.dir/src/ApolloConfig.cpp.o.d -o CMakeFiles/Apollocppclient.dir/src/ApolloConfig.cpp.o -c /home/aldd/JD/apollo_cpp_sdk/src/ApolloConfig.cpp

CMakeFiles/Apollocppclient.dir/src/ApolloConfig.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Apollocppclient.dir/src/ApolloConfig.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/aldd/JD/apollo_cpp_sdk/src/ApolloConfig.cpp > CMakeFiles/Apollocppclient.dir/src/ApolloConfig.cpp.i

CMakeFiles/Apollocppclient.dir/src/ApolloConfig.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Apollocppclient.dir/src/ApolloConfig.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/aldd/JD/apollo_cpp_sdk/src/ApolloConfig.cpp -o CMakeFiles/Apollocppclient.dir/src/ApolloConfig.cpp.s

CMakeFiles/Apollocppclient.dir/src/Executor.cpp.o: CMakeFiles/Apollocppclient.dir/flags.make
CMakeFiles/Apollocppclient.dir/src/Executor.cpp.o: ../src/Executor.cpp
CMakeFiles/Apollocppclient.dir/src/Executor.cpp.o: CMakeFiles/Apollocppclient.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/aldd/JD/apollo_cpp_sdk/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/Apollocppclient.dir/src/Executor.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/Apollocppclient.dir/src/Executor.cpp.o -MF CMakeFiles/Apollocppclient.dir/src/Executor.cpp.o.d -o CMakeFiles/Apollocppclient.dir/src/Executor.cpp.o -c /home/aldd/JD/apollo_cpp_sdk/src/Executor.cpp

CMakeFiles/Apollocppclient.dir/src/Executor.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Apollocppclient.dir/src/Executor.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/aldd/JD/apollo_cpp_sdk/src/Executor.cpp > CMakeFiles/Apollocppclient.dir/src/Executor.cpp.i

CMakeFiles/Apollocppclient.dir/src/Executor.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Apollocppclient.dir/src/Executor.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/aldd/JD/apollo_cpp_sdk/src/Executor.cpp -o CMakeFiles/Apollocppclient.dir/src/Executor.cpp.s

CMakeFiles/Apollocppclient.dir/src/HttpClient.cpp.o: CMakeFiles/Apollocppclient.dir/flags.make
CMakeFiles/Apollocppclient.dir/src/HttpClient.cpp.o: ../src/HttpClient.cpp
CMakeFiles/Apollocppclient.dir/src/HttpClient.cpp.o: CMakeFiles/Apollocppclient.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/aldd/JD/apollo_cpp_sdk/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/Apollocppclient.dir/src/HttpClient.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/Apollocppclient.dir/src/HttpClient.cpp.o -MF CMakeFiles/Apollocppclient.dir/src/HttpClient.cpp.o.d -o CMakeFiles/Apollocppclient.dir/src/HttpClient.cpp.o -c /home/aldd/JD/apollo_cpp_sdk/src/HttpClient.cpp

CMakeFiles/Apollocppclient.dir/src/HttpClient.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Apollocppclient.dir/src/HttpClient.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/aldd/JD/apollo_cpp_sdk/src/HttpClient.cpp > CMakeFiles/Apollocppclient.dir/src/HttpClient.cpp.i

CMakeFiles/Apollocppclient.dir/src/HttpClient.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Apollocppclient.dir/src/HttpClient.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/aldd/JD/apollo_cpp_sdk/src/HttpClient.cpp -o CMakeFiles/Apollocppclient.dir/src/HttpClient.cpp.s

CMakeFiles/Apollocppclient.dir/src/MultiServerAppidCluster.cpp.o: CMakeFiles/Apollocppclient.dir/flags.make
CMakeFiles/Apollocppclient.dir/src/MultiServerAppidCluster.cpp.o: ../src/MultiServerAppidCluster.cpp
CMakeFiles/Apollocppclient.dir/src/MultiServerAppidCluster.cpp.o: CMakeFiles/Apollocppclient.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/aldd/JD/apollo_cpp_sdk/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/Apollocppclient.dir/src/MultiServerAppidCluster.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/Apollocppclient.dir/src/MultiServerAppidCluster.cpp.o -MF CMakeFiles/Apollocppclient.dir/src/MultiServerAppidCluster.cpp.o.d -o CMakeFiles/Apollocppclient.dir/src/MultiServerAppidCluster.cpp.o -c /home/aldd/JD/apollo_cpp_sdk/src/MultiServerAppidCluster.cpp

CMakeFiles/Apollocppclient.dir/src/MultiServerAppidCluster.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Apollocppclient.dir/src/MultiServerAppidCluster.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/aldd/JD/apollo_cpp_sdk/src/MultiServerAppidCluster.cpp > CMakeFiles/Apollocppclient.dir/src/MultiServerAppidCluster.cpp.i

CMakeFiles/Apollocppclient.dir/src/MultiServerAppidCluster.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Apollocppclient.dir/src/MultiServerAppidCluster.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/aldd/JD/apollo_cpp_sdk/src/MultiServerAppidCluster.cpp -o CMakeFiles/Apollocppclient.dir/src/MultiServerAppidCluster.cpp.s

CMakeFiles/Apollocppclient.dir/src/RemoteConfigLongPollService.cpp.o: CMakeFiles/Apollocppclient.dir/flags.make
CMakeFiles/Apollocppclient.dir/src/RemoteConfigLongPollService.cpp.o: ../src/RemoteConfigLongPollService.cpp
CMakeFiles/Apollocppclient.dir/src/RemoteConfigLongPollService.cpp.o: CMakeFiles/Apollocppclient.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/aldd/JD/apollo_cpp_sdk/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/Apollocppclient.dir/src/RemoteConfigLongPollService.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/Apollocppclient.dir/src/RemoteConfigLongPollService.cpp.o -MF CMakeFiles/Apollocppclient.dir/src/RemoteConfigLongPollService.cpp.o.d -o CMakeFiles/Apollocppclient.dir/src/RemoteConfigLongPollService.cpp.o -c /home/aldd/JD/apollo_cpp_sdk/src/RemoteConfigLongPollService.cpp

CMakeFiles/Apollocppclient.dir/src/RemoteConfigLongPollService.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Apollocppclient.dir/src/RemoteConfigLongPollService.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/aldd/JD/apollo_cpp_sdk/src/RemoteConfigLongPollService.cpp > CMakeFiles/Apollocppclient.dir/src/RemoteConfigLongPollService.cpp.i

CMakeFiles/Apollocppclient.dir/src/RemoteConfigLongPollService.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Apollocppclient.dir/src/RemoteConfigLongPollService.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/aldd/JD/apollo_cpp_sdk/src/RemoteConfigLongPollService.cpp -o CMakeFiles/Apollocppclient.dir/src/RemoteConfigLongPollService.cpp.s

CMakeFiles/Apollocppclient.dir/src/Threadpool.cpp.o: CMakeFiles/Apollocppclient.dir/flags.make
CMakeFiles/Apollocppclient.dir/src/Threadpool.cpp.o: ../src/Threadpool.cpp
CMakeFiles/Apollocppclient.dir/src/Threadpool.cpp.o: CMakeFiles/Apollocppclient.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/aldd/JD/apollo_cpp_sdk/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object CMakeFiles/Apollocppclient.dir/src/Threadpool.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/Apollocppclient.dir/src/Threadpool.cpp.o -MF CMakeFiles/Apollocppclient.dir/src/Threadpool.cpp.o.d -o CMakeFiles/Apollocppclient.dir/src/Threadpool.cpp.o -c /home/aldd/JD/apollo_cpp_sdk/src/Threadpool.cpp

CMakeFiles/Apollocppclient.dir/src/Threadpool.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Apollocppclient.dir/src/Threadpool.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/aldd/JD/apollo_cpp_sdk/src/Threadpool.cpp > CMakeFiles/Apollocppclient.dir/src/Threadpool.cpp.i

CMakeFiles/Apollocppclient.dir/src/Threadpool.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Apollocppclient.dir/src/Threadpool.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/aldd/JD/apollo_cpp_sdk/src/Threadpool.cpp -o CMakeFiles/Apollocppclient.dir/src/Threadpool.cpp.s

CMakeFiles/Apollocppclient.dir/src/common.cpp.o: CMakeFiles/Apollocppclient.dir/flags.make
CMakeFiles/Apollocppclient.dir/src/common.cpp.o: ../src/common.cpp
CMakeFiles/Apollocppclient.dir/src/common.cpp.o: CMakeFiles/Apollocppclient.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/aldd/JD/apollo_cpp_sdk/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Building CXX object CMakeFiles/Apollocppclient.dir/src/common.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/Apollocppclient.dir/src/common.cpp.o -MF CMakeFiles/Apollocppclient.dir/src/common.cpp.o.d -o CMakeFiles/Apollocppclient.dir/src/common.cpp.o -c /home/aldd/JD/apollo_cpp_sdk/src/common.cpp

CMakeFiles/Apollocppclient.dir/src/common.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Apollocppclient.dir/src/common.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/aldd/JD/apollo_cpp_sdk/src/common.cpp > CMakeFiles/Apollocppclient.dir/src/common.cpp.i

CMakeFiles/Apollocppclient.dir/src/common.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Apollocppclient.dir/src/common.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/aldd/JD/apollo_cpp_sdk/src/common.cpp -o CMakeFiles/Apollocppclient.dir/src/common.cpp.s

# Object files for target Apollocppclient
Apollocppclient_OBJECTS = \
"CMakeFiles/Apollocppclient.dir/src/ApolloConfig.cpp.o" \
"CMakeFiles/Apollocppclient.dir/src/Executor.cpp.o" \
"CMakeFiles/Apollocppclient.dir/src/HttpClient.cpp.o" \
"CMakeFiles/Apollocppclient.dir/src/MultiServerAppidCluster.cpp.o" \
"CMakeFiles/Apollocppclient.dir/src/RemoteConfigLongPollService.cpp.o" \
"CMakeFiles/Apollocppclient.dir/src/Threadpool.cpp.o" \
"CMakeFiles/Apollocppclient.dir/src/common.cpp.o"

# External object files for target Apollocppclient
Apollocppclient_EXTERNAL_OBJECTS =

../lib/libApollocppclient.so: CMakeFiles/Apollocppclient.dir/src/ApolloConfig.cpp.o
../lib/libApollocppclient.so: CMakeFiles/Apollocppclient.dir/src/Executor.cpp.o
../lib/libApollocppclient.so: CMakeFiles/Apollocppclient.dir/src/HttpClient.cpp.o
../lib/libApollocppclient.so: CMakeFiles/Apollocppclient.dir/src/MultiServerAppidCluster.cpp.o
../lib/libApollocppclient.so: CMakeFiles/Apollocppclient.dir/src/RemoteConfigLongPollService.cpp.o
../lib/libApollocppclient.so: CMakeFiles/Apollocppclient.dir/src/Threadpool.cpp.o
../lib/libApollocppclient.so: CMakeFiles/Apollocppclient.dir/src/common.cpp.o
../lib/libApollocppclient.so: CMakeFiles/Apollocppclient.dir/build.make
../lib/libApollocppclient.so: /usr/lib/x86_64-linux-gnu/libcurl.so
../lib/libApollocppclient.so: /usr/local/lib/liblog4cpp.so
../lib/libApollocppclient.so: /usr/lib/x86_64-linux-gnu/libcurl.so
../lib/libApollocppclient.so: /usr/local/lib/liblog4cpp.so
../lib/libApollocppclient.so: CMakeFiles/Apollocppclient.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/aldd/JD/apollo_cpp_sdk/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_8) "Linking CXX shared library ../lib/libApollocppclient.so"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/Apollocppclient.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/Apollocppclient.dir/build: ../lib/libApollocppclient.so
.PHONY : CMakeFiles/Apollocppclient.dir/build

CMakeFiles/Apollocppclient.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/Apollocppclient.dir/cmake_clean.cmake
.PHONY : CMakeFiles/Apollocppclient.dir/clean

CMakeFiles/Apollocppclient.dir/depend:
	cd /home/aldd/JD/apollo_cpp_sdk/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/aldd/JD/apollo_cpp_sdk /home/aldd/JD/apollo_cpp_sdk /home/aldd/JD/apollo_cpp_sdk/build /home/aldd/JD/apollo_cpp_sdk/build /home/aldd/JD/apollo_cpp_sdk/build/CMakeFiles/Apollocppclient.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/Apollocppclient.dir/depend

