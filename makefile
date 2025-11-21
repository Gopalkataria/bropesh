#######################################
#    makefile for os mini-project     #
#    made with <3 by Gopal Kataria    #
#######################################

cc = gcc
# compiler flags
# -I. looks for header files in the root directory
# -Isrc looks for header files in the src directory
cflags = -Wall -Wextra -pedantic -std=c99 -I. -Isrc

# directories
src_dir = src
build_dir = build

# automatically find all .c files in src/ (e.g., src/main.c src/prompt.c ...)
src_files = $(wildcard $(src_dir)/*.c)

# generate the list of object files in build/ by replacing prefix and extension
# (e.g., src/main.c -> build/main.o)
obj_files = $(patsubst $(src_dir)/%.c, $(build_dir)/%.o, $(src_files))

target = bropesh

# default target
all: $(target)

# rule to link object files into the final executable
$(target): $(obj_files)
	$(cc) $(cflags) $(obj_files) -o $(target)

# rule to compile source files into object files
# this matches any file "build/filename.o" and finds "src/filename.c"
# @mkdir -p $(build_dir) creates the directory if it doesn't exist
$(build_dir)/%.o: $(src_dir)/%.c shell.h
	@mkdir -p $(build_dir)
	$(cc) $(cflags) -c $< -o $@

# clean target: removes the build folder and the executable
clean:
	rm -rf $(build_dir) $(target)
	-rm -f $(HOME)/.our_shell_history
