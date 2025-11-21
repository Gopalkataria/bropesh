#######################################
#    makefile for os mini-project     #
#    made with <3 by Gopal Kataria    #
#######################################

cc = gcc
# compiler - all warnings, extra warnings, pedantic, c99 standard
cflags = -Wall -Wextra -pedantic -std=c99

src_files = main.c prompt.c builtins.c history.c process.c signal_handlers.c utils.c
obj_files = $(src_files:.c=.o)

target = bropesh

all: $(target)

# rule to link object files into the executable
$(target): $(obj_files)
	$(cc) $(cflags) $(obj_files) -o $(target)

# rule to compile each c source file into an object file
# depends on its .c file and the shell.h header
%.o: %.c shell.h
	$(cc) $(cflags) -c $< -o $@

# clean target: removes generated object files and the executable
clean:
	rm -f $(obj_files) $(target)
	-rm -f $(HOME)/.our_shell_history
