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
include src/CMakeFiles/control.dir/depend.make

# Include the progress variables for this target.
include src/CMakeFiles/control.dir/progress.make

# Include the compile flags for this target's objects.
include src/CMakeFiles/control.dir/flags.make

src/CMakeFiles/control.dir/Control.cc.o: src/CMakeFiles/control.dir/flags.make
src/CMakeFiles/control.dir/Control.cc.o: ../src/Control.cc
	$(CMAKE_COMMAND) -E cmake_progress_report /home/xiaochao/project/system/push/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object src/CMakeFiles/control.dir/Control.cc.o"
	cd /home/xiaochao/project/system/push/build/src && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/control.dir/Control.cc.o -c /home/xiaochao/project/system/push/src/Control.cc

src/CMakeFiles/control.dir/Control.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/control.dir/Control.cc.i"
	cd /home/xiaochao/project/system/push/build/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/xiaochao/project/system/push/src/Control.cc > CMakeFiles/control.dir/Control.cc.i

src/CMakeFiles/control.dir/Control.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/control.dir/Control.cc.s"
	cd /home/xiaochao/project/system/push/build/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/xiaochao/project/system/push/src/Control.cc -o CMakeFiles/control.dir/Control.cc.s

src/CMakeFiles/control.dir/Control.cc.o.requires:
.PHONY : src/CMakeFiles/control.dir/Control.cc.o.requires

src/CMakeFiles/control.dir/Control.cc.o.provides: src/CMakeFiles/control.dir/Control.cc.o.requires
	$(MAKE) -f src/CMakeFiles/control.dir/build.make src/CMakeFiles/control.dir/Control.cc.o.provides.build
.PHONY : src/CMakeFiles/control.dir/Control.cc.o.provides

src/CMakeFiles/control.dir/Control.cc.o.provides.build: src/CMakeFiles/control.dir/Control.cc.o

src/CMakeFiles/control.dir/MysqlCDriver.cc.o: src/CMakeFiles/control.dir/flags.make
src/CMakeFiles/control.dir/MysqlCDriver.cc.o: ../src/MysqlCDriver.cc
	$(CMAKE_COMMAND) -E cmake_progress_report /home/xiaochao/project/system/push/build/CMakeFiles $(CMAKE_PROGRESS_2)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object src/CMakeFiles/control.dir/MysqlCDriver.cc.o"
	cd /home/xiaochao/project/system/push/build/src && /usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/control.dir/MysqlCDriver.cc.o -c /home/xiaochao/project/system/push/src/MysqlCDriver.cc

src/CMakeFiles/control.dir/MysqlCDriver.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/control.dir/MysqlCDriver.cc.i"
	cd /home/xiaochao/project/system/push/build/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/xiaochao/project/system/push/src/MysqlCDriver.cc > CMakeFiles/control.dir/MysqlCDriver.cc.i

src/CMakeFiles/control.dir/MysqlCDriver.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/control.dir/MysqlCDriver.cc.s"
	cd /home/xiaochao/project/system/push/build/src && /usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/xiaochao/project/system/push/src/MysqlCDriver.cc -o CMakeFiles/control.dir/MysqlCDriver.cc.s

src/CMakeFiles/control.dir/MysqlCDriver.cc.o.requires:
.PHONY : src/CMakeFiles/control.dir/MysqlCDriver.cc.o.requires

src/CMakeFiles/control.dir/MysqlCDriver.cc.o.provides: src/CMakeFiles/control.dir/MysqlCDriver.cc.o.requires
	$(MAKE) -f src/CMakeFiles/control.dir/build.make src/CMakeFiles/control.dir/MysqlCDriver.cc.o.provides.build
.PHONY : src/CMakeFiles/control.dir/MysqlCDriver.cc.o.provides

src/CMakeFiles/control.dir/MysqlCDriver.cc.o.provides.build: src/CMakeFiles/control.dir/MysqlCDriver.cc.o

# Object files for target control
control_OBJECTS = \
"CMakeFiles/control.dir/Control.cc.o" \
"CMakeFiles/control.dir/MysqlCDriver.cc.o"

# External object files for target control
control_EXTERNAL_OBJECTS =

src/control: src/CMakeFiles/control.dir/Control.cc.o
src/control: src/CMakeFiles/control.dir/MysqlCDriver.cc.o
src/control: src/CMakeFiles/control.dir/build.make
src/control: src/CMakeFiles/control.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable control"
	cd /home/xiaochao/project/system/push/build/src && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/control.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
src/CMakeFiles/control.dir/build: src/control
.PHONY : src/CMakeFiles/control.dir/build

src/CMakeFiles/control.dir/requires: src/CMakeFiles/control.dir/Control.cc.o.requires
src/CMakeFiles/control.dir/requires: src/CMakeFiles/control.dir/MysqlCDriver.cc.o.requires
.PHONY : src/CMakeFiles/control.dir/requires

src/CMakeFiles/control.dir/clean:
	cd /home/xiaochao/project/system/push/build/src && $(CMAKE_COMMAND) -P CMakeFiles/control.dir/cmake_clean.cmake
.PHONY : src/CMakeFiles/control.dir/clean

src/CMakeFiles/control.dir/depend:
	cd /home/xiaochao/project/system/push/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/xiaochao/project/system/push /home/xiaochao/project/system/push/src /home/xiaochao/project/system/push/build /home/xiaochao/project/system/push/build/src /home/xiaochao/project/system/push/build/src/CMakeFiles/control.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : src/CMakeFiles/control.dir/depend

