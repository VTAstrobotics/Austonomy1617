# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.5

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/dev/ros_code/src

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/dev/ros_code/build

# Include any dependencies generated for this target.
include test/CMakeFiles/runme.dir/depend.make

# Include the progress variables for this target.
include test/CMakeFiles/runme.dir/progress.make

# Include the compile flags for this target's objects.
include test/CMakeFiles/runme.dir/flags.make

test/CMakeFiles/runme.dir/hello.cpp.o: test/CMakeFiles/runme.dir/flags.make
test/CMakeFiles/runme.dir/hello.cpp.o: /home/dev/ros_code/src/test/hello.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/dev/ros_code/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object test/CMakeFiles/runme.dir/hello.cpp.o"
	cd /home/dev/ros_code/build/test && /usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/runme.dir/hello.cpp.o -c /home/dev/ros_code/src/test/hello.cpp

test/CMakeFiles/runme.dir/hello.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/runme.dir/hello.cpp.i"
	cd /home/dev/ros_code/build/test && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/dev/ros_code/src/test/hello.cpp > CMakeFiles/runme.dir/hello.cpp.i

test/CMakeFiles/runme.dir/hello.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/runme.dir/hello.cpp.s"
	cd /home/dev/ros_code/build/test && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/dev/ros_code/src/test/hello.cpp -o CMakeFiles/runme.dir/hello.cpp.s

test/CMakeFiles/runme.dir/hello.cpp.o.requires:

.PHONY : test/CMakeFiles/runme.dir/hello.cpp.o.requires

test/CMakeFiles/runme.dir/hello.cpp.o.provides: test/CMakeFiles/runme.dir/hello.cpp.o.requires
	$(MAKE) -f test/CMakeFiles/runme.dir/build.make test/CMakeFiles/runme.dir/hello.cpp.o.provides.build
.PHONY : test/CMakeFiles/runme.dir/hello.cpp.o.provides

test/CMakeFiles/runme.dir/hello.cpp.o.provides.build: test/CMakeFiles/runme.dir/hello.cpp.o


# Object files for target runme
runme_OBJECTS = \
"CMakeFiles/runme.dir/hello.cpp.o"

# External object files for target runme
runme_EXTERNAL_OBJECTS =

test/runme: test/CMakeFiles/runme.dir/hello.cpp.o
test/runme: test/CMakeFiles/runme.dir/build.make
test/runme: /opt/ros/kinetic/lib/libroscpp.so
test/runme: /usr/lib/x86_64-linux-gnu/libboost_signals.so
test/runme: /usr/lib/x86_64-linux-gnu/libboost_filesystem.so
test/runme: /opt/ros/kinetic/lib/librosconsole.so
test/runme: /opt/ros/kinetic/lib/librosconsole_log4cxx.so
test/runme: /opt/ros/kinetic/lib/librosconsole_backend_interface.so
test/runme: /usr/lib/x86_64-linux-gnu/liblog4cxx.so
test/runme: /usr/lib/x86_64-linux-gnu/libboost_regex.so
test/runme: /opt/ros/kinetic/lib/libroscpp_serialization.so
test/runme: /opt/ros/kinetic/lib/librostime.so
test/runme: /opt/ros/kinetic/lib/libxmlrpcpp.so
test/runme: /opt/ros/kinetic/lib/libcpp_common.so
test/runme: /usr/lib/x86_64-linux-gnu/libboost_system.so
test/runme: /usr/lib/x86_64-linux-gnu/libboost_thread.so
test/runme: /usr/lib/x86_64-linux-gnu/libboost_chrono.so
test/runme: /usr/lib/x86_64-linux-gnu/libboost_date_time.so
test/runme: /usr/lib/x86_64-linux-gnu/libboost_atomic.so
test/runme: /usr/lib/x86_64-linux-gnu/libpthread.so
test/runme: /usr/lib/x86_64-linux-gnu/libconsole_bridge.so
test/runme: test/CMakeFiles/runme.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/dev/ros_code/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable runme"
	cd /home/dev/ros_code/build/test && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/runme.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
test/CMakeFiles/runme.dir/build: test/runme

.PHONY : test/CMakeFiles/runme.dir/build

test/CMakeFiles/runme.dir/requires: test/CMakeFiles/runme.dir/hello.cpp.o.requires

.PHONY : test/CMakeFiles/runme.dir/requires

test/CMakeFiles/runme.dir/clean:
	cd /home/dev/ros_code/build/test && $(CMAKE_COMMAND) -P CMakeFiles/runme.dir/cmake_clean.cmake
.PHONY : test/CMakeFiles/runme.dir/clean

test/CMakeFiles/runme.dir/depend:
	cd /home/dev/ros_code/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/dev/ros_code/src /home/dev/ros_code/src/test /home/dev/ros_code/build /home/dev/ros_code/build/test /home/dev/ros_code/build/test/CMakeFiles/runme.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : test/CMakeFiles/runme.dir/depend

