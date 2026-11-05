# SchedLab Glossary

Short definitions of terms we use across our docs and code.

## Shell and Process Basics

**Shell**: A program that reads commands from the user and runs them. Bash, Dash, Zsh, and SchedLab are all shells.

**REPL**: Read, Eval, Print Loop. The main loop of a shell. It shows a prompt, reads a line, runs it, and repeats.

**Process**: A running program with its own memory and resources. Every command the shell runs becomes a new process.

**fork()**: A system call that creates a new process by copying the calling process. The original is the parent and the copy is the child.

**execvp()**: A system call that replaces the current process image with a new program. The shell calls it in the child after fork to run the command.

**waitpid()**: A system call that lets a parent wait for a child to finish and collect its exit status.

**Zombie process**: A child that has finished but has not been reaped by its parent with wait or waitpid. It still holds an entry in the process table.

**Built-in command**: A command the shell runs itself instead of in a child process. cd must be a built-in, because changing directory in a child does not affect the shell.

**File descriptor**: A small integer the kernel uses to refer to an open file, pipe, or terminal. 0 is standard input, 1 is standard output, 2 is standard error.

**Pipe**: A one-way channel between two processes. In ls | wc, the output of ls becomes the input of wc.

**Redirection**: Sending a command's input or output to a file instead of the terminal, using >, <, >>, or 2>. The shell does this with dup2().

**Signal**: A small notification the kernel sends to a process, such as SIGINT when the user presses Ctrl+C.

**SIGCHLD**: The signal the kernel sends to a parent when one of its children stops or exits. SchedLab uses it to know when a background slot opens up.

**Async-signal-safe**: Describes functions that are safe to call inside a signal handler. Most library functions, like printf and malloc, are not.

## Jobs and Scheduling

**Job**: A command line the shell is running, which may include several processes in a pipeline.

**Foreground job**: A job the shell waits for before showing the next prompt.

**Background job**: A job started with & at the end. The shell shows the prompt right away while the job keeps running.

**Job control**: Shell features for managing jobs, such as jobs, fg, bg, and Ctrl+Z.

**Process group**: A set of related processes that receive signals together. Shells put each job in its own process group so Ctrl+C only hits the foreground job.

**Kernel scheduler**: The part of the operating system that decides which runnable process gets the CPU next. Linux has used EEVDF since kernel 6.6, and CFS before that.

**Concurrency limit (N)**: The maximum number of background jobs SchedLab lets run at the same time. Set with setconcurrency N. Extra jobs wait in a queue.

**Scheduling policy**: The rule SchedLab uses to pick which queued job starts next when a slot opens.

**FIFO**: First In, First Out. Jobs start in the order they arrived.

**SJF**: Shortest Job First. Jobs with the shortest predicted runtime start first. This lowers average wait time but can make long jobs wait a long time.

**Priority scheduling**: Jobs with higher priority start first. In SchedLab, priority comes from the nice value.

**nice value**: A Unix priority hint from -20 (highest priority) to 19 (lowest). It can be set with nice or setpriority().

**Starvation**: When a job waits forever because other jobs keep getting picked first.

**Aging**: Slowly raising the priority of a job the longer it waits, so it cannot starve.

**EMA (Exponential Moving Average)**: A running average that weights recent values more. SchedLab uses it to predict how long a command will take based on past runs.

**AIMD (Additive Increase, Multiplicative Decrease)**: A feedback rule borrowed from TCP congestion control. SchedLab adds 1 to N when the foreground is fast enough, and cuts N in half when it is too slow.

**Adaptive policy**: SchedLab's policy that adjusts N on its own using AIMD, based on a target set with setlatency.

## Metrics

**Foreground latency**: How long a foreground command, like ls, takes to finish while background jobs are running. Measured in milliseconds.

**Background throughput**: How many background jobs finish per second.

**Wait time**: How long a job sits in the queue before it starts.

**Turnaround time**: Total time from when a job is submitted until it finishes, including wait time.

**Slowdown**: Turnaround time divided by the time the job would take if it ran alone.

**Jain's Fairness Index**: A score that shows how evenly resources are shared across n jobs. It ranges from 1/n (very unfair) to 1 (perfectly fair).

**CPU core / vCPU**: A processing unit that can run one thread at a time. A VM's vCPU count matters for our experiments, because a concurrency limit that matches the core count could make one policy look better than it really is.
