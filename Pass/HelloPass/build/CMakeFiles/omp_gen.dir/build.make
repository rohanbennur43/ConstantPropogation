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
CMAKE_SOURCE_DIR = /home/ubuntu/rohan/CS201-F24-Template/Pass/HelloPass

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/ubuntu/rohan/CS201-F24-Template/Pass/HelloPass/build

# Utility rule file for omp_gen.

# Include any custom commands dependencies for this target.
include CMakeFiles/omp_gen.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/omp_gen.dir/progress.make

omp_gen: CMakeFiles/omp_gen.dir/build.make
.PHONY : omp_gen

# Rule to build all files generated by this target.
CMakeFiles/omp_gen.dir/build: omp_gen
.PHONY : CMakeFiles/omp_gen.dir/build

CMakeFiles/omp_gen.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/omp_gen.dir/cmake_clean.cmake
.PHONY : CMakeFiles/omp_gen.dir/clean

CMakeFiles/omp_gen.dir/depend:
	cd /home/ubuntu/rohan/CS201-F24-Template/Pass/HelloPass/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/ubuntu/rohan/CS201-F24-Template/Pass/HelloPass /home/ubuntu/rohan/CS201-F24-Template/Pass/HelloPass /home/ubuntu/rohan/CS201-F24-Template/Pass/HelloPass/build /home/ubuntu/rohan/CS201-F24-Template/Pass/HelloPass/build /home/ubuntu/rohan/CS201-F24-Template/Pass/HelloPass/build/CMakeFiles/omp_gen.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/omp_gen.dir/depend

