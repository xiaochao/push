# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

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

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/xiaochao/project/system/push

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/xiaochao/project/system/push/build

# Include any dependencies generated for this target.
include src/CMakeFiles/server.dir/depend.make

# Include the progress variables for this target.
include src/CMakeFiles/server.dir/progress.make

# Include the compile flags for this target's objects.
include src/CMakeFiles/server.dir/flags.make

src/CMakeFiles/server.dir/Server.cc.o: src/CMakeFiles/server.dir/flags.make
src/CMakeFiles/server.dir/Server.cc.o: ../src/Server.cc
	$(CMAKE_COMMAND) -E cmake_progress_report /home/xiaochao/project/system/push/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object src/CMakeFiles/server.dir/Server.cc.o"
	cd /home/xiaochao/project/system/push/build/src && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/server.dir/Server.cc.o -c /home/xiaochao/project/system/push/src/Server.cc

src/CMakeFiles/server.dir/Server.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/server.dir/Server.cc.i"
	cd /home/xiaochao/project/system/push/build/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/xiaochao/project/system/push/src/Server.cc > CMakeFiles/server.dir/Server.cc.i

src/CMakeFiles/server.dir/Server.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/server.dir/Server.cc.s"
	cd /home/xiaochao/project/system/push/build/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/xiaochao/project/system/push/src/Server.cc -o CMakeFiles/server.dir/Server.cc.s

src/CMakeFiles/server.dir/Server.cc.o.requires:
.PHONY : src/CMakeFiles/server.dir/Server.cc.o.requires

src/CMakeFiles/server.dir/Server.cc.o.provides: src/CMakeFiles/server.dir/Server.cc.o.requires
	$(MAKE) -f src/CMakeFiles/server.dir/build.make src/CMakeFiles/server.dir/Server.cc.o.provides.build
.PHONY : src/CMakeFiles/server.dir/Server.cc.o.provides

src/CMakeFiles/server.dir/Server.cc.o.provides.build: src/CMakeFiles/server.dir/Server.cc.o

src/CMakeFiles/server.dir/MysqlCDriver.cc.o: src/CMakeFiles/server.dir/flags.make
src/CMakeFiles/server.dir/MysqlCDriver.cc.o: ../src/MysqlCDriver.cc
	$(CMAKE_COMMAND) -E cmake_progress_report /home/xiaochao/project/system/push/build/CMakeFiles $(CMAKE_PROGRESS_2)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object src/CMakeFiles/server.dir/MysqlCDriver.cc.o"
	cd /home/xiaochao/project/system/push/build/src && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/server.dir/MysqlCDriver.cc.o -c /home/xiaochao/project/system/push/src/MysqlCDriver.cc

src/CMakeFiles/server.dir/MysqlCDriver.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/server.dir/MysqlCDriver.cc.i"
	cd /home/xiaochao/project/system/push/build/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/xiaochao/project/system/push/src/MysqlCDriver.cc > CMakeFiles/server.dir/MysqlCDriver.cc.i

src/CMakeFiles/server.dir/MysqlCDriver.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/server.dir/MysqlCDriver.cc.s"
	cd /home/xiaochao/project/system/push/build/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/xiaochao/project/system/push/src/MysqlCDriver.cc -o CMakeFiles/server.dir/MysqlCDriver.cc.s

src/CMakeFiles/server.dir/MysqlCDriver.cc.o.requires:
.PHONY : src/CMakeFiles/server.dir/MysqlCDriver.cc.o.requires

src/CMakeFiles/server.dir/MysqlCDriver.cc.o.provides: src/CMakeFiles/server.dir/MysqlCDriver.cc.o.requires
	$(MAKE) -f src/CMakeFiles/server.dir/build.make src/CMakeFiles/server.dir/MysqlCDriver.cc.o.provides.build
.PHONY : src/CMakeFiles/server.dir/MysqlCDriver.cc.o.provides

src/CMakeFiles/server.dir/MysqlCDriver.cc.o.provides.build: src/CMakeFiles/server.dir/MysqlCDriver.cc.o

src/CMakeFiles/server.dir/OperateChar.cc.o: src/CMakeFiles/server.dir/flags.make
src/CMakeFiles/server.dir/OperateChar.cc.o: ../src/OperateChar.cc
	$(CMAKE_COMMAND) -E cmake_progress_report /home/xiaochao/project/system/push/build/CMakeFiles $(CMAKE_PROGRESS_3)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object src/CMakeFiles/server.dir/OperateChar.cc.o"
	cd /home/xiaochao/project/system/push/build/src && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/server.dir/OperateChar.cc.o -c /home/xiaochao/project/system/push/src/OperateChar.cc

src/CMakeFiles/server.dir/OperateChar.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/server.dir/OperateChar.cc.i"
	cd /home/xiaochao/project/system/push/build/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/xiaochao/project/system/push/src/OperateChar.cc > CMakeFiles/server.dir/OperateChar.cc.i

src/CMakeFiles/server.dir/OperateChar.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/server.dir/OperateChar.cc.s"
	cd /home/xiaochao/project/system/push/build/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/xiaochao/project/system/push/src/OperateChar.cc -o CMakeFiles/server.dir/OperateChar.cc.s

src/CMakeFiles/server.dir/OperateChar.cc.o.requires:
.PHONY : src/CMakeFiles/server.dir/OperateChar.cc.o.requires

src/CMakeFiles/server.dir/OperateChar.cc.o.provides: src/CMakeFiles/server.dir/OperateChar.cc.o.requires
	$(MAKE) -f src/CMakeFiles/server.dir/build.make src/CMakeFiles/server.dir/OperateChar.cc.o.provides.build
.PHONY : src/CMakeFiles/server.dir/OperateChar.cc.o.provides

src/CMakeFiles/server.dir/OperateChar.cc.o.provides.build: src/CMakeFiles/server.dir/OperateChar.cc.o

# Object files for target server
server_OBJECTS = \
"CMakeFiles/server.dir/Server.cc.o" \
"CMakeFiles/server.dir/MysqlCDriver.cc.o" \
"CMakeFiles/server.dir/OperateChar.cc.o"

# External object files for target server
server_EXTERNAL_OBJECTS =

src/server: src/CMakeFiles/server.dir/Server.cc.o
src/server: src/CMakeFiles/server.dir/MysqlCDriver.cc.o
src/server: src/CMakeFiles/server.dir/OperateChar.cc.o
src/server: src/CMakeFiles/server.dir/build.make
src/server: src/CMakeFiles/server.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable server"
	cd /home/xiaochao/project/system/push/build/src && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/server.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/CMakeFiles/server.dir/build: src/server
.PHONY : src/CMakeFiles/server.dir/build

src/CMakeFiles/server.dir/requires: src/CMakeFiles/server.dir/Server.cc.o.requires
src/CMakeFiles/server.dir/requires: src/CMakeFiles/server.dir/MysqlCDriver.cc.o.requires
src/CMakeFiles/server.dir/requires: src/CMakeFiles/server.dir/OperateChar.cc.o.requires
.PHONY : src/CMakeFiles/server.dir/requires

src/CMakeFiles/server.dir/clean:
	cd /home/xiaochao/project/system/push/build/src && $(CMAKE_COMMAND) -P CMakeFiles/server.dir/cmake_clean.cmake
.PHONY : src/CMakeFiles/server.dir/clean

src/CMakeFiles/server.dir/depend:
	cd /home/xiaochao/project/system/push/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/xiaochao/project/system/push /home/xiaochao/project/system/push/src /home/xiaochao/project/system/push/build /home/xiaochao/project/system/push/build/src /home/xiaochao/project/system/push/build/src/CMakeFiles/server.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/CMakeFiles/server.dir/depend

