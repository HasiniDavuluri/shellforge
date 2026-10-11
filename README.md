# 🐚 ShellForge — Custom Unix Shell in C

**ShellForge** is a custom Unix-like command-line shell developed in C for Linux. It provides a simple command execution environment with built-in commands, command history, aliases, and support for shell operations.

## ✨ Features

* **Command Execution** — Execute external Linux commands.
* **Built-in Commands** — Supports `cd`, `pwd`, `echo`, and `exit`.
* **Additional Built-ins** — Includes `help`, `env`, `which`, `clear`, and `hostname`.
* **Command History** — View previously entered commands using `history`.
* **History Navigation** — Navigate through previous commands using the Up and Down arrow keys.
* **Repeat Previous Command** — Execute the previous command using `!!`.
* **Aliases** — Create command shortcuts, list aliases, and execute aliased commands.
* **Variable Expansion** — Expand supported environment variables.
* **Interactive Interface** — Uses GNU Readline for convenient command-line input.

## 🛠️ Technologies Used

* **Language:** C
* **Operating System:** Linux / Ubuntu
* **Compiler:** GCC
* **Build Tool:** GNU Make
* **Libraries:** GNU Readline
* **Version Control:** Git and GitHub

## 📁 Project Structure

```text
shellforge/
├── include/
│   ├── builtin.h
│   ├── execute.h
│   ├── expand.h
│   ├── history.h
│   └── parser.h
├── src/
│   ├── builtin.c
│   ├── execute.c
│   ├── expand.c
│   ├── history.c
│   ├── main.c
│   ├── parser.c
│   └── token.c
├── tests/
├── docs/
├── lexer.c
├── lexer.h
├── makefile
└── README.md
```

*Note: The directory structure above reflects the intended organization; update it if your actual repository differs.*

## ⚙️ Installation and Setup

### 1. Clone the Repository

```bash
git clone YOUR_REPOSITORY_URL
```

Replace `YOUR_REPOSITORY_URL` with your GitHub repository URL.

### 2. Navigate to the Project Directory

```bash
cd shellforge
```

### 3. Install Dependencies

On Ubuntu or Debian:

```bash
sudo apt update
sudo apt install gcc make libreadline-dev
```

### 4. Compile the Project

```bash
make
```

To clean the generated executable before rebuilding:

```bash
make clean
make
```

### 5. Run ShellForge

```bash
./shellforge
```

You should see the ShellForge banner and interactive prompt.

## 💻 Usage

After launching ShellForge, try the following commands.

### Basic Commands

```bash
pwd
ls
echo Hello, ShellForge!
cd ..
hostname
```

### Built-in Help

```bash
help
```

### Environment Variables

```bash
env
```

### Locate an Executable

```bash
which ls
```

### Command History

```bash
history
```

Use the **Up** and **Down** arrow keys to navigate command history.

Repeat the previous command:

```bash
!!
```

### Aliases

Create a shortcut:

```bash
alias ll='ls -l'
```

Execute the shortcut:

```bash
ll
```

List defined aliases:

```bash
alias
```

### Exit the Shell

```bash
exit
```

## 🧠 How It Works

ShellForge follows a basic shell execution workflow:

1. **Read Input** — Read commands interactively using GNU Readline.
2. **Lexical Analysis** — Tokenize the input into command components.
3. **Parsing** — Parse tokens into a command or pipeline representation.
4. **Variable Expansion** — Expand supported environment variables.
5. **Command Execution** — Execute built-in or external commands.
6. **History Management** — Maintain command history for convenient navigation and reuse.

## 🎯 Project Objectives

* Understand how Unix shells process commands.
* Learn C programming and Linux system calls.
* Implement command parsing and execution.
* Explore environment variables and process management.
* Practice modular software development and debugging.
* Gain practical experience with Git and GitHub.

## 🚧 Future Improvements

Potential future enhancements include:

* Input and output redirection (`<`, `>`, `>>`).
* Pipelines using `|`.
* Background execution using `&`.
* Conditional execution using `&&` and `||`.
* Improved error handling.
* Automated test cases.
* Persistent command history across sessions.

These features should be marked as completed only after they have been implemented and tested.

## 🤝 Contributing

Contributions and suggestions are welcome. You can fork the repository, create a feature branch, and submit a pull request.

## 📄 License

This project is available for educational and personal use. Add a license file if you intend to distribute it under a specific open-source license.

---

**Developed as a systems programming project using C and Linux.**
