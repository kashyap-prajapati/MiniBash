# MiniBash

Sure, here is a README file for the GitHub project of the `shell24` shell program:

---

# shell24

`shell24` is a simple shell program written in C that executes commands entered by the user. It supports various functionalities such as creating new shell sessions, handling command piping, file redirection, background execution, conditional execution, and sequential execution.

## Features

- **New Shell Sessions:** Create a new shell24 session using the command `newt`.
- **Command Execution:** Supports commands with arguments (1 <= argc <= 5).
- **Piping:** Supports up to 6 piping operations.
- **Redirection:** Supports input (`<`), output (`>`), and append (`>>`) redirection.
- **Background Execution:** Run commands in the background using `&`.
- **Conditional Execution:** Supports conditional execution with `&&` and `||`.
- **Sequential Execution:** Execute commands sequentially using `;`.
- **File Concatenation:** Concatenate up to 5 text files using `#`.

## Getting Started

### Prerequisites

- GCC compiler
- Unix-like operating system (Linux, macOS)

### Installation

1. Clone the repository:

    ```sh
    git clone https://github.com/your-username/shell24.git
    cd shell24
    ```

2. Compile the program:

    ```sh
    gcc shell24.c -o shell24
    ```

### Usage

Start the shell by running the compiled program:

```sh
./shell24
```

### Example Commands

- Start a new shell session:

    ```sh
    shell24$newt
    ```

- Execute a command with arguments:

    ```sh
    shell24$ ls -l -t ~/chapter5/dir1
    ```

- Piping:

    ```sh
    shell24$ ls | grep *.c | wc | wc -w
    ```

- Redirection:

    ```sh
    shell24$ cat new.txt >> sample.txt
    ```

- Background Execution:

    ```sh
    shell24$ ex1 &
    shell24$ fg
    ```

- Conditional Execution:

    ```sh
    shell24$ ex1 && ex2 || ex3 && ex4
    ```

- Sequential Execution:

    ```sh
    shell24$ ls -l -t ; date ; ex1 ;
    ```

- File Concatenation:

    ```sh
    shell24$ check.txt # new.txt # new1.txt # sample.txt
    ```

## Implementation Details

The shell uses the following system calls to execute commands:

- `fork()`: To create a child process.
- `execvp()`: To replace the child process image with a new process image.
- `waitpid()`: To wait for the child process to finish.
- `dup2()`: To duplicate file descriptors for redirection.
- `pipe()`: To create pipes for inter-process communication.

### Code Structure

- `main()`: The main loop that reads commands from the user.
- `execute_command()`: Executes a single command.
- `handle_piping()`: Handles commands with pipes.
- `handle_redirection()`: Handles commands with redirection.
- `handle_background()`: Handles background execution.
- `handle_sequential()`: Handles sequential execution.
- `handle_conditional()`: Handles conditional execution.
- `concatenate_files()`: Concatenates multiple files.
- `new_shell()`: Creates a new shell24 session.

## Contributing

Contributions are welcome! Please open an issue or submit a pull request for any improvements or bug fixes.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgements

- This project was inspired by the need for a simple, modular shell implementation for educational purposes.

