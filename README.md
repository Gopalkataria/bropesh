

```
  █████               2023112006 OSA Project              █████        
  ▒▒███                                                   ▒▒███         
   ▒███████  ████████   ██████  ████████   ██████   █████  ▒███████     
   ▒███▒▒███▒▒███▒▒███ ███▒▒███▒▒███▒▒███ ███▒▒███ ███▒▒   ▒███▒▒███    
   ▒███ ▒███ ▒███ ▒▒▒ ▒███ ▒███ ▒███ ▒███▒███████ ▒▒█████  ▒███ ▒███    
   ▒███ ▒███ ▒███     ▒███ ▒███ ▒███ ▒███▒███▒▒▒   ▒▒▒▒███ ▒███ ▒███    
   ████████  █████    ▒▒██████  ▒███████ ▒▒██████  ██████  ████ █████   
  ▒▒▒▒▒▒▒▒  ▒▒▒▒▒      ▒▒▒▒▒▒   ▒███▒▒▒   ▒▒▒▒▒▒  ▒▒▒▒▒▒  ▒▒▒▒ ▒▒▒▒▒    
         \    /\                ▒███  -_-_-_-_-_-_-_-_-_-_-_-_-_-_-
          )  ( ')               ████                                   
         (  /  )                ▒▒▒▒  Crafted with ♡ by Gopal Kataria 
          \(__)|      
                     
```

# Operating Systems Mini-Project: Bropesh Shell

**Name:** Gopal Kataria  
**Roll Number:** 2023112006  
**Course:** Algorithms and Operating Systems 

---

##  Introduction

**Bropesh** is a custom, interactive command-line interpreter (shell) written in C. It is designed to mimic the functionality of standard shells like Bash or Zsh. It supports built-in commands, external process execution, input/output redirection, background processing, and persistent command history.
---

## Project Structure

 The project is organized to ensure modularity and clean compilation.

```text
.
├── makefile              # Build automation configuration
├── shell.h               # Main header file (structs, global vars, prototypes)
├── bropesh               # Final executable (generated after make)
├── README.md             # Documentation
├── src/                  # Source code directory
│   ├── main.c
│   ├── prompt.c
│   ├── builtins.c
│   ├── history.c
│   ├── process.c
│   ├── signal_handlers.c
│   └── utils.c
└── build/                # Object files (.o) directory (generated during build)
    ├── main.o
    ...
```

---

## Compilation & Execution

This project uses a **Makefile** to handle compilation and linking. Instructions : 

### 1. Compile the Shell
To compile the program and generate the `bropesh` executable:
```bash
make
```
*Or explicitly:*
```bash
make all
```

### 2. Run the Shell
Once compiled, execute the shell:
```bash
./bropesh
```

### 3. Clean Build Files
To remove the `bropesh` executable and the `build/` directory (and object files):
```bash
make clean
```

---

##  File Implementation Details

The code is split into multiple modules to ensure logical separation of concerns.

| File | Description |
| :--- | :--- |
| **`src/main.c`** | The entry point of the shell. It runs the REPL (Read-Eval-Print Loop), initializes signal handlers, loads history, and coordinates the execution flow. |
| **`shell.h`** | The shared header file. It contains all standard library imports, macro definitions (like `MAX_ARGS`), global variable declarations (like `home_dir`), and function prototypes used across the project. |
| **`src/prompt.c`** | Handles the display of the shell prompt. It fetches the username, hostname, and current working directory (cwd). It creates a relative path display (replacing home path with `~`) and applies ANSI color codes/ligatures. |
| **`src/builtins.c`** | Implements commands that must run within the shell process itself. Includes logic for `cd`, `pwd`, `echo`, `history`, `help`, and `exit`. |
| **`src/process.c`** | Manages external command execution. It handles `fork()`, `execvp()`, and `waitpid()`. It also contains the logic for **I/O Redirection** (`dup2`) and running processes in the **background** (not waiting for child). |
| **`src/history.c`** | Manages the persistence of commands. Reads from and writes to a hidden file (`.our_shell_history`) in the user's home directory. Uses a circular buffer logic to store the last 20 unique commands. |
| **`src/signal_handlers.c`** | Defines custom signal handling behavior. Captures `SIGINT` (Ctrl+C) to prevent the shell from crashing and `SIGCHLD` to clean up "zombie" background processes asynchronously. |
| **`src/utils.c`** | Contains string manipulation utilities. Primarily responsible for `tokenize_input`, which parses the user string, handling spaces, tabs, quotes (`""`), and special tokens like `&`, `<`, and `>`. |

---

##  Features Implemented

1.  **Shell Prompt:** Displays `<user@system:path>` with support for `~` relative path and colored formatting.
2.  **Built-in Commands:**
    *   `cd`: Change directory (supports `..`, `~`, `-`).
    *   `pwd`: Print working directory.
    *   `echo`: Print arguments to standard output.
    *   `history`: View last 20 commands.
    *   `help`: Display help menu.
    *   `exit`: Cleanly terminate the shell.
3.  **External Commands:** Can run system programs (e.g., `ls -la`, `vim`, `grep`).
4.  **I/O Redirection:**
    *   Input: `command < input.txt`
    *   Output: `command > output.txt`
5.  **Background Processes:** Execute commands in the background using `&` (e.g., `docker-compose build &`).
6.  **Signal Handling:**
    *   `Ctrl+C`: Interrupts foreground process but keeps shell alive.
    *   `Ctrl+D`: Logs out/exits the shell.
7.  **History Persistence:** History is saved to `~/.bropesh_history` and reloaded on next session.
