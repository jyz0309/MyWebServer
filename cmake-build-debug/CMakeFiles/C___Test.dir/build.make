# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.15

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /home/alkaid/.local/share/JetBrains/Toolbox/apps/CLion/ch-0/192.7142.39/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /home/alkaid/.local/share/JetBrains/Toolbox/apps/CLion/ch-0/192.7142.39/bin/cmake/linux/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /data/C++_Test

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /data/C++_Test/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/C___Test.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/C___Test.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/C___Test.dir/flags.make

CMakeFiles/C___Test.dir/main.cpp.o: CMakeFiles/C___Test.dir/flags.make
CMakeFiles/C___Test.dir/main.cpp.o: ../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/data/C++_Test/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/C___Test.dir/main.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/C___Test.dir/main.cpp.o -c /data/C++_Test/main.cpp

CMakeFiles/C___Test.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/C___Test.dir/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /data/C++_Test/main.cpp > CMakeFiles/C___Test.dir/main.cpp.i

CMakeFiles/C___Test.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/C___Test.dir/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /data/C++_Test/main.cpp -o CMakeFiles/C___Test.dir/main.cpp.s

CMakeFiles/C___Test.dir/epollIO.cpp.o: CMakeFiles/C___Test.dir/flags.make
CMakeFiles/C___Test.dir/epollIO.cpp.o: ../epollIO.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/data/C++_Test/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/C___Test.dir/epollIO.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/C___Test.dir/epollIO.cpp.o -c /data/C++_Test/epollIO.cpp

CMakeFiles/C___Test.dir/epollIO.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/C___Test.dir/epollIO.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /data/C++_Test/epollIO.cpp > CMakeFiles/C___Test.dir/epollIO.cpp.i

CMakeFiles/C___Test.dir/epollIO.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/C___Test.dir/epollIO.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /data/C++_Test/epollIO.cpp -o CMakeFiles/C___Test.dir/epollIO.cpp.s

CMakeFiles/C___Test.dir/epoll_server.cpp.o: CMakeFiles/C___Test.dir/flags.make
CMakeFiles/C___Test.dir/epoll_server.cpp.o: ../epoll_server.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/data/C++_Test/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/C___Test.dir/epoll_server.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/C___Test.dir/epoll_server.cpp.o -c /data/C++_Test/epoll_server.cpp

CMakeFiles/C___Test.dir/epoll_server.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/C___Test.dir/epoll_server.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /data/C++_Test/epoll_server.cpp > CMakeFiles/C___Test.dir/epoll_server.cpp.i

CMakeFiles/C___Test.dir/epoll_server.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/C___Test.dir/epoll_server.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /data/C++_Test/epoll_server.cpp -o CMakeFiles/C___Test.dir/epoll_server.cpp.s

CMakeFiles/C___Test.dir/threadpool.cpp.o: CMakeFiles/C___Test.dir/flags.make
CMakeFiles/C___Test.dir/threadpool.cpp.o: ../threadpool.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/data/C++_Test/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/C___Test.dir/threadpool.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/C___Test.dir/threadpool.cpp.o -c /data/C++_Test/threadpool.cpp

CMakeFiles/C___Test.dir/threadpool.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/C___Test.dir/threadpool.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /data/C++_Test/threadpool.cpp > CMakeFiles/C___Test.dir/threadpool.cpp.i

CMakeFiles/C___Test.dir/threadpool.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/C___Test.dir/threadpool.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /data/C++_Test/threadpool.cpp -o CMakeFiles/C___Test.dir/threadpool.cpp.s

# Object files for target C___Test
C___Test_OBJECTS = \
"CMakeFiles/C___Test.dir/main.cpp.o" \
"CMakeFiles/C___Test.dir/epollIO.cpp.o" \
"CMakeFiles/C___Test.dir/epoll_server.cpp.o" \
"CMakeFiles/C___Test.dir/threadpool.cpp.o"

# External object files for target C___Test
C___Test_EXTERNAL_OBJECTS =

C___Test: CMakeFiles/C___Test.dir/main.cpp.o
C___Test: CMakeFiles/C___Test.dir/epollIO.cpp.o
C___Test: CMakeFiles/C___Test.dir/epoll_server.cpp.o
C___Test: CMakeFiles/C___Test.dir/threadpool.cpp.o
C___Test: CMakeFiles/C___Test.dir/build.make
C___Test: CMakeFiles/C___Test.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/data/C++_Test/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Linking CXX executable C___Test"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/C___Test.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/C___Test.dir/build: C___Test

.PHONY : CMakeFiles/C___Test.dir/build

CMakeFiles/C___Test.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/C___Test.dir/cmake_clean.cmake
.PHONY : CMakeFiles/C___Test.dir/clean

CMakeFiles/C___Test.dir/depend:
	cd /data/C++_Test/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /data/C++_Test /data/C++_Test /data/C++_Test/cmake-build-debug /data/C++_Test/cmake-build-debug /data/C++_Test/cmake-build-debug/CMakeFiles/C___Test.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/C___Test.dir/depend
