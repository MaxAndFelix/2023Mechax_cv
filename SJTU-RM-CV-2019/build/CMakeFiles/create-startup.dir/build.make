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
CMAKE_SOURCE_DIR = /home/mechax/documents/2023Mechax_cv/SJTU-RM-CV-2019

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/mechax/documents/2023Mechax_cv/SJTU-RM-CV-2019/build

# Utility rule file for create-startup.

# Include any custom commands dependencies for this target.
include CMakeFiles/create-startup.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/create-startup.dir/progress.make

CMakeFiles/create-startup:
	../tools/create-startup.sh /home/mechax/documents/2023Mechax_cv/SJTU-RM-CV-2019 /home/mechax/documents/2023Mechax_cv/SJTU-RM-CV-2019/build

create-startup: CMakeFiles/create-startup
create-startup: CMakeFiles/create-startup.dir/build.make
.PHONY : create-startup

# Rule to build all files generated by this target.
CMakeFiles/create-startup.dir/build: create-startup
.PHONY : CMakeFiles/create-startup.dir/build

CMakeFiles/create-startup.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/create-startup.dir/cmake_clean.cmake
.PHONY : CMakeFiles/create-startup.dir/clean

CMakeFiles/create-startup.dir/depend:
	cd /home/mechax/documents/2023Mechax_cv/SJTU-RM-CV-2019/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/mechax/documents/2023Mechax_cv/SJTU-RM-CV-2019 /home/mechax/documents/2023Mechax_cv/SJTU-RM-CV-2019 /home/mechax/documents/2023Mechax_cv/SJTU-RM-CV-2019/build /home/mechax/documents/2023Mechax_cv/SJTU-RM-CV-2019/build /home/mechax/documents/2023Mechax_cv/SJTU-RM-CV-2019/build/CMakeFiles/create-startup.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/create-startup.dir/depend

