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
CMAKE_COMMAND = /home/geronimonassois/.local/share/JetBrains/Toolbox/apps/CLion/ch-0/193.6015.37/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /home/geronimonassois/.local/share/JetBrains/Toolbox/apps/CLion/ch-0/193.6015.37/bin/cmake/linux/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/geronimonassois/Data/Workspace/Uni/WS2019/Konzepte_systemnaher_Programmierung/njvm/vm

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/geronimonassois/Data/Workspace/Uni/WS2019/Konzepte_systemnaher_Programmierung/njvm/vm/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/njvm.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/njvm.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/njvm.dir/flags.make

CMakeFiles/njvm.dir/njvm.c.o: CMakeFiles/njvm.dir/flags.make
CMakeFiles/njvm.dir/njvm.c.o: ../njvm.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/geronimonassois/Data/Workspace/Uni/WS2019/Konzepte_systemnaher_Programmierung/njvm/vm/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/njvm.dir/njvm.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/njvm.dir/njvm.c.o   -c /home/geronimonassois/Data/Workspace/Uni/WS2019/Konzepte_systemnaher_Programmierung/njvm/vm/njvm.c

CMakeFiles/njvm.dir/njvm.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/njvm.dir/njvm.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/geronimonassois/Data/Workspace/Uni/WS2019/Konzepte_systemnaher_Programmierung/njvm/vm/njvm.c > CMakeFiles/njvm.dir/njvm.c.i

CMakeFiles/njvm.dir/njvm.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/njvm.dir/njvm.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/geronimonassois/Data/Workspace/Uni/WS2019/Konzepte_systemnaher_Programmierung/njvm/vm/njvm.c -o CMakeFiles/njvm.dir/njvm.c.s

CMakeFiles/njvm.dir/debugger/debugger.c.o: CMakeFiles/njvm.dir/flags.make
CMakeFiles/njvm.dir/debugger/debugger.c.o: ../debugger/debugger.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/geronimonassois/Data/Workspace/Uni/WS2019/Konzepte_systemnaher_Programmierung/njvm/vm/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/njvm.dir/debugger/debugger.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/njvm.dir/debugger/debugger.c.o   -c /home/geronimonassois/Data/Workspace/Uni/WS2019/Konzepte_systemnaher_Programmierung/njvm/vm/debugger/debugger.c

CMakeFiles/njvm.dir/debugger/debugger.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/njvm.dir/debugger/debugger.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/geronimonassois/Data/Workspace/Uni/WS2019/Konzepte_systemnaher_Programmierung/njvm/vm/debugger/debugger.c > CMakeFiles/njvm.dir/debugger/debugger.c.i

CMakeFiles/njvm.dir/debugger/debugger.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/njvm.dir/debugger/debugger.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/geronimonassois/Data/Workspace/Uni/WS2019/Konzepte_systemnaher_Programmierung/njvm/vm/debugger/debugger.c -o CMakeFiles/njvm.dir/debugger/debugger.c.s

# Object files for target njvm
njvm_OBJECTS = \
"CMakeFiles/njvm.dir/njvm.c.o" \
"CMakeFiles/njvm.dir/debugger/debugger.c.o"

# External object files for target njvm
njvm_EXTERNAL_OBJECTS =

njvm: CMakeFiles/njvm.dir/njvm.c.o
njvm: CMakeFiles/njvm.dir/debugger/debugger.c.o
njvm: CMakeFiles/njvm.dir/build.make
njvm: CMakeFiles/njvm.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/geronimonassois/Data/Workspace/Uni/WS2019/Konzepte_systemnaher_Programmierung/njvm/vm/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking C executable njvm"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/njvm.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/njvm.dir/build: njvm

.PHONY : CMakeFiles/njvm.dir/build

CMakeFiles/njvm.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/njvm.dir/cmake_clean.cmake
.PHONY : CMakeFiles/njvm.dir/clean

CMakeFiles/njvm.dir/depend:
	cd /home/geronimonassois/Data/Workspace/Uni/WS2019/Konzepte_systemnaher_Programmierung/njvm/vm/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/geronimonassois/Data/Workspace/Uni/WS2019/Konzepte_systemnaher_Programmierung/njvm/vm /home/geronimonassois/Data/Workspace/Uni/WS2019/Konzepte_systemnaher_Programmierung/njvm/vm /home/geronimonassois/Data/Workspace/Uni/WS2019/Konzepte_systemnaher_Programmierung/njvm/vm/cmake-build-debug /home/geronimonassois/Data/Workspace/Uni/WS2019/Konzepte_systemnaher_Programmierung/njvm/vm/cmake-build-debug /home/geronimonassois/Data/Workspace/Uni/WS2019/Konzepte_systemnaher_Programmierung/njvm/vm/cmake-build-debug/CMakeFiles/njvm.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/njvm.dir/depend

