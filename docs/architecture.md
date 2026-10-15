# SchedLab – System Architecture (Deliverable 4)

SchedLab extends a standard UNIX shell with a scheduling subsystem for background jobs. Instead of starting every `&` job immediately, the shell enforces a concurrency limit **N** (`setconcurrency N`) and uses a scheduling policy (FIFO, SJF, Priority, or Adaptive) to choose which queued job starts when a slot opens.

## Components

| Component | Responsibility |
|---|---|
| Shell REPL | Reads input, runs the main loop, handles foreground jobs and built-ins |
| Parser | Turns a command line into a job (argv, pipes, redirects, background flag) |
| Scheduler | Tracks running jobs, enforces limit N, dispatches queued jobs |
| Policy Queue | Min-heap keyed by arrival time (FIFO), predicted runtime (SJF), or nice value with aging (Priority) |
| History Table | Stores an EMA of past runtimes per command, used by SJF |
| SIGCHLD Handler | Reaps finished children with `waitpid(WNOHANG)` and notifies the main loop |
| Adaptive Controller | AIMD loop that adjusts N based on foreground latency vs. `setlatency` target |
| Metrics Logger | Writes foreground latency, wait time, and turnaround to CSV; backs the `stats` command |

## Critical Path 1: Submitting a Background Job

When the user starts a background job, the scheduler either launches it right away (a slot is free) or places it in the policy queue.

```mermaid
sequenceDiagram
    actor User
    participant Shell as Shell REPL
    participant Parser
    participant Sched as Scheduler
    participant Hist as History Table (EMA)
    participant Queue as Policy Queue (Min-Heap)
    participant Kernel as OS Kernel
    participant Child as Child Process

    User->>Shell: types "./work.sh &"
    Shell->>Parser: parse(line)
    Parser-->>Shell: Job (argv, background = true)
    Shell->>Sched: submit(job)
    Sched->>Sched: record arrival time
    alt running jobs below limit N
        Sched->>Kernel: fork()
        Kernel->>Child: start child process
        Child->>Child: setpgid(), execvp(argv)
        opt Priority policy
            Sched->>Kernel: setpriority(pid, nice)
        end
        Sched->>Sched: running += 1, add to job table
        Shell-->>User: [1] pid running
    else limit N reached
        opt SJF policy
            Sched->>Hist: lookup(command)
            Hist-->>Sched: predicted runtime (EMA)
        end
        Sched->>Queue: push(job, key)
        Note over Queue: key = arrival time (FIFO),<br/>predicted runtime (SJF),<br/>nice value with aging (Priority)
        Shell-->>User: [1] queued
    end
    Shell-->>User: prompt returns immediately
```

## Critical Path 2: Job Completion and Dispatch

When a background job exits, the shell reaps it, records its metrics, and fills the freed slot from the queue.

```mermaid
sequenceDiagram
    participant Child as Child Process
    participant Kernel as OS Kernel
    participant Handler as SIGCHLD Handler
    participant Shell as Shell REPL
    participant Sched as Scheduler
    participant Hist as History Table (EMA)
    participant Metrics as Metrics Logger (CSV)
    participant Queue as Policy Queue

    Child->>Kernel: exit(status)
    Kernel->>Handler: deliver SIGCHLD
    loop while waitpid(-1, WNOHANG) returns a pid
        Handler->>Handler: save pid, status, end time
    end
    Handler->>Shell: set child_exited flag
    Note over Handler,Shell: The handler only reaps and sets a flag.<br/>Scheduling work runs in the main loop<br/>to stay async-signal-safe.
    Shell->>Sched: on_job_finished(pid, end time)
    Sched->>Sched: running -= 1, mark job done
    Sched->>Hist: update EMA for this command
    Note over Hist: new = alpha x actual + (1 - alpha) x old
    Sched->>Metrics: log wait time, turnaround, slowdown
    loop while running below N and queue not empty
        Sched->>Queue: pop()
        Queue-->>Sched: next job chosen by policy
        Sched->>Kernel: fork(), execvp()
        Sched->>Sched: running += 1, record start time
    end
```

## Critical Path 3: Adaptive Concurrency Control (AIMD)

Under the Adaptive policy, every foreground command is timed, and the controller adjusts N toward the user's latency target, similar to TCP congestion control.

```mermaid
sequenceDiagram
    actor User
    participant Shell as Shell REPL
    participant Kernel as OS Kernel
    participant Metrics as Metrics Logger
    participant AIMD as Adaptive Controller (AIMD)
    participant Sched as Scheduler

    User->>Shell: setlatency 50
    Shell->>AIMD: target = 50 ms
    User->>Shell: types "ls"
    Shell->>Shell: t0 = clock_gettime()
    Shell->>Kernel: fork(), execvp("ls"), waitpid()
    Kernel-->>Shell: foreground job finished
    Shell->>Shell: latency = now - t0
    Shell->>Metrics: log foreground latency
    Shell->>AIMD: report(latency)
    alt latency above target
        AIMD->>AIMD: N = max(1, N / 2)
        AIMD->>Sched: set limit N
        Note over Sched: Running jobs are not killed.<br/>New jobs wait until running drops below N.
    else latency at or below target
        AIMD->>AIMD: N = N + 1
        AIMD->>Sched: set limit N
        Sched->>Sched: dispatch queued jobs into the new slot
    end
```
