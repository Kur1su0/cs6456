# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

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
CMAKE_SOURCE_DIR = /home/zw9ga/git/p2-concurrency/exp1

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/zw9ga/git/p2-concurrency/exp1

# Include any dependencies generated for this target.
include CMakeFiles/counter.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/counter.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/counter.dir/flags.make

CMakeFiles/counter.dir/counter.c.o: CMakeFiles/counter.dir/flags.make
CMakeFiles/counter.dir/counter.c.o: counter.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/zw9ga/git/p2-concurrency/exp1/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/counter.dir/counter.c.o"
	gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/counter.dir/counter.c.o   -c /home/zw9ga/git/p2-concurrency/exp1/counter.c

CMakeFiles/counter.dir/counter.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/counter.dir/counter.c.i"
	gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/zw9ga/git/p2-concurrency/exp1/counter.c > CMakeFiles/counter.dir/counter.c.i

CMakeFiles/counter.dir/counter.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/counter.dir/counter.c.s"
	gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/zw9ga/git/p2-concurrency/exp1/counter.c -o CMakeFiles/counter.dir/counter.c.s

# Object files for target counter
counter_OBJECTS = \
"CMakeFiles/counter.dir/counter.c.o"

# External object files for target counter
counter_EXTERNAL_OBJECTS =

counter: CMakeFiles/counter.dir/counter.c.o
counter: CMakeFiles/counter.dir/build.make
counter: CMakeFiles/counter.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/zw9ga/git/p2-concurrency/exp1/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable counter"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/counter.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/counter.dir/build: counter

.PHONY : CMakeFiles/counter.dir/build

CMakeFiles/counter.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/counter.dir/cmake_clean.cmake
.PHONY : CMakeFiles/counter.dir/clean

CMakeFiles/counter.dir/depend:
	cd /home/zw9ga/git/p2-concurrency/exp1 && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/zw9ga/git/p2-concurrency/exp1 /home/zw9ga/git/p2-concurrency/exp1 /home/zw9ga/git/p2-concurrency/exp1 /home/zw9ga/git/p2-concurrency/exp1 /home/zw9ga/git/p2-concurrency/exp1/CMakeFiles/counter.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/counter.dir/depend

