# SchedLab: A Research Shell for Job Scheduling Experiments

**Comparing FIFO, SJF, Priority, and Adaptive Scheduling Policies in a UNIX Shell**

## Team
- Isobella Gordon — gordon.i@northeastern.edu
- Patrick Araujo — araujo.pa@northeastern.edu
- Xueyan Zhang — zhang.xueya@northeastern.edu

## Project Overview
SchedLab is a UNIX shell that limits concurrent background jobs and compares four scheduling policies to reduce foreground latency under heavy load.

## Research Question
Can a UNIX shell that limits concurrent background jobs reduce foreground command latency compared to unlimited execution (bash, zsh, fish)?

## Language
SchedLab is written in **C** (C11 with GNU extensions, compiled with gcc). We chose C because it maps directly to the POSIX process API (fork, execvp, waitpid, signals) used throughout OSTEP and most reference shell implementations.

## Build and Run
Requirements: Linux, gcc, and GNU make. Tested on Ubuntu 24.04.

```bash
make            # build the schedlab binary
./schedlab      # start the shell
make clean      # remove build files
```

Inside the shell, type a command such as `ls -l` and press Enter. Press Ctrl+D to exit.

## Current Status (Sprint 2 Skeleton)
- REPL loop that reads a line of input and shows a prompt
- Parser that splits a command into arguments and detects pipes, redirects, and `&`
- External commands run in a child process with fork and execvp, and the shell waits with waitpid
- Built-in commands (exit, cd, pwd) and research extension commands (setconcurrency, setlatency) are being added in `src/builtins.c`
- Pipes, redirects, and background jobs print a not supported yet message for now

## Deliverables
**Sprint 1**
- [Team Bio](docs/team-bio.pdf)
- [Exploration Report](docs/exploration.md)
- [Research Proposal](docs/research-proposal.pdf)
- [System Architecture](docs/architecture.md)

**Sprint 2**
- [Annotated Bibliography](docs/bibliography.md)
- [Glossary](docs/glossary.md)
- [AI Reflection Log](docs/ai-reflections.md)

## Contribution Tracker
[Link to Google Sheet](https://docs.google.com/spreadsheets/d/1lmQa8dNrhZjUqUDc2SUEsn8ZcfMSkHmpjOKQ7rzyxPE/edit?gid=0#gid=0)

## Repository Structure
- `Makefile` — Build rules
- `src/main.c` — REPL loop
- `src/parser.c`, `src/parser.h` — Command parsing
- `src/exec.c`, `src/exec.h` — fork/exec execution
- `src/builtins.c`, `src/builtins.h` — Built-in commands
- `docs/` — Project documents
- `docs/screenshots/` — Exploration screenshots

## Open-Source Code
No external open-source code is used so far. All code in `src/` was written by the team.
