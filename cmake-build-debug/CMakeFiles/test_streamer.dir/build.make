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
CMAKE_COMMAND = /home/abed/clion-2019.2.1/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /home/abed/clion-2019.2.1/bin/cmake/linux/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/abed/Desktop/projects/rtsp_encoder_decoder

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/abed/Desktop/projects/rtsp_encoder_decoder/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/test_streamer.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/test_streamer.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/test_streamer.dir/flags.make

CMakeFiles/test_streamer.dir/test_streamer.cpp.o: CMakeFiles/test_streamer.dir/flags.make
CMakeFiles/test_streamer.dir/test_streamer.cpp.o: ../test_streamer.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/abed/Desktop/projects/rtsp_encoder_decoder/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/test_streamer.dir/test_streamer.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/test_streamer.dir/test_streamer.cpp.o -c /home/abed/Desktop/projects/rtsp_encoder_decoder/test_streamer.cpp

CMakeFiles/test_streamer.dir/test_streamer.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/test_streamer.dir/test_streamer.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/abed/Desktop/projects/rtsp_encoder_decoder/test_streamer.cpp > CMakeFiles/test_streamer.dir/test_streamer.cpp.i

CMakeFiles/test_streamer.dir/test_streamer.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/test_streamer.dir/test_streamer.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/abed/Desktop/projects/rtsp_encoder_decoder/test_streamer.cpp -o CMakeFiles/test_streamer.dir/test_streamer.cpp.s

# Object files for target test_streamer
test_streamer_OBJECTS = \
"CMakeFiles/test_streamer.dir/test_streamer.cpp.o"

# External object files for target test_streamer
test_streamer_EXTERNAL_OBJECTS =

test_streamer: CMakeFiles/test_streamer.dir/test_streamer.cpp.o
test_streamer: CMakeFiles/test_streamer.dir/build.make
test_streamer: librtsp_encoder_decoder.so
test_streamer: /usr/lib/x86_64-linux-gnu/libgstrtspserver-1.0.so
test_streamer: /usr/lib/x86_64-linux-gnu/libgstrtsp-1.0.so
test_streamer: CMakeFiles/test_streamer.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/abed/Desktop/projects/rtsp_encoder_decoder/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable test_streamer"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/test_streamer.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/test_streamer.dir/build: test_streamer

.PHONY : CMakeFiles/test_streamer.dir/build

CMakeFiles/test_streamer.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/test_streamer.dir/cmake_clean.cmake
.PHONY : CMakeFiles/test_streamer.dir/clean

CMakeFiles/test_streamer.dir/depend:
	cd /home/abed/Desktop/projects/rtsp_encoder_decoder/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/abed/Desktop/projects/rtsp_encoder_decoder /home/abed/Desktop/projects/rtsp_encoder_decoder /home/abed/Desktop/projects/rtsp_encoder_decoder/cmake-build-debug /home/abed/Desktop/projects/rtsp_encoder_decoder/cmake-build-debug /home/abed/Desktop/projects/rtsp_encoder_decoder/cmake-build-debug/CMakeFiles/test_streamer.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/test_streamer.dir/depend

