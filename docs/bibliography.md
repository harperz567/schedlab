# SchedLab Bibliography

## Research Extension Update

Our hypothesis did not change: a small limit (around the number of CPU cores) should cut foreground latency with little throughput loss, and Adaptive should match the best fixed limit without manual tuning. Gregg [7] is consistent with the first part, because his busy-system example shows run-queue waits in the tens of milliseconds, and Caladan [3] supports the second, because it shows that a controller that reacts quickly to interference keeps latency-critical work fast. The GNU make discussion [10] added one caution: a load limit set above what the signal can reach lets every job run, so we could consider an upper bound for N in the Adaptive policy.

---

## Academic Papers

### [1] D. M. Ritchie and K. Thompson, "The UNIX Time-Sharing System," The Bell System Technical Journal 57(6), 1978 (revised from CACM 17(7), 1974). [Link](https://www.nokia.com/bell-labs/about/dennis-m-ritchie/cacm.html)

**Category:** Academic Paper

**Summary:** This is the original UNIX paper. It describes the shell, fork/exec/wait, pipes, and I/O redirection. It also explains that a trailing `&` makes the shell skip the wait and show the prompt again immediately, which is the exact behavior SchedLab changes.

**Relevance:** SchedLab keeps the same fork/exec/wait model but adds a queue between parsing the line and forking the child. In the original shell every `&` job starts at once with no limit, so this paper is the baseline we are extending.

### [2] J.-P. Lozi, B. Lepers, J. Funston, F. Gaud, V. Quéma, and A. Fedorova, "The Linux Scheduler: a Decade of Wasted Cores," EuroSys, 2016. [Link](https://people.ece.ubc.ca/~sasha/papers/eurosys16-final29.pdf)

**Category:** Academic Paper

**Summary:** The authors found four bugs in the Linux CFS scheduler that left cores idle while runnable threads waited in other run queues. They built a sanity checker that flags "idle core next to waiting thread" and a tool that visualizes run-queue sizes over time. On some workloads the bugs caused many-fold slowdowns.

**Relevance:** SchedLab assumes the kernel spreads our N background jobs across cores well. This paper shows that assumption can fail, so our benchmarks must record per-core usage, not just total time. Their "idle core while threads wait" check is also a good sanity test for our own job queue.

### [3] J. Fried, Z. Ruan, A. Ousterhout, and A. Belay, "Caladan: Mitigating Interference at Microsecond Timescales," OSDI, 2020. [Link](https://www.usenix.org/system/files/osdi20-fried.pdf)

**Category:** Academic Paper

**Summary:** Caladan is a CPU scheduler that keeps a latency-critical app (memcached) fast while a best-effort job runs on the same machine. Instead of partitioning cores and caches, it watches for interference and moves cores between tasks within microseconds. When resource use shifted, it cut tail latency from 580 ms to 52 µs compared with PARTIES, a partitioning system that adjusts about every 500 ms.

**Relevance:** This is SchedLab's problem at a much finer scale: foreground commands are latency-critical and background `&` jobs are best-effort. The paper's main point is that how fast the controller reacts matters, not only what it decides. Our AIMD loop reacts after every foreground command, so it can adjust N quickly when load changes.

---

## Technical Books

### [4] W. R. Stevens and S. A. Rago, "Chapter 9: Process Relationships," in Advanced Programming in the UNIX Environment, 3rd ed., Addison-Wesley, 2013. [Link](https://learning.oreilly.com/library/view/advanced-programming-in/9780321638014/ch09.html)

**Category:** Book

**Summary:** Chapter 9 covers process groups, sessions, the controlling terminal, and `tcsetpgrp`. Section 9.8 explains job control, and Section 9.9 shows how a shell runs programs in the foreground and background. This is the reference for how a shell keeps the terminal owned by the foreground job.

**Relevance:** SchedLab must put each background job in its own process group and keep the foreground group in control of the terminal. Sections 9.8 and 9.9 are the reference for this, so a queued job never steals the terminal from the user.

### [5] M. Kerrisk, "Chapter 34: Process Groups, Sessions, and Job Control," in The Linux Programming Interface, No Starch Press, 2010. [Link](https://learning.oreilly.com/library/view/the-linux-programming/9781593272203/xhtml/ch34.xhtml)

**Category:** Book

**Summary:** Chapter 34 explains process groups, sessions, foreground and background groups, SIGHUP handling, and how a shell implements job control (Section 34.7.2). It also covers job-control signals such as SIGTSTP, SIGTTIN, and SIGCONT.

**Relevance:** This is the Linux-specific companion to [4]. Section 34.7.2 matches what SchedLab does when it starts a queued job later: set the process group and decide who owns the terminal.

---

## Engineering Blogs / Industry Deep-Dives

### [6] J. Corbet, "An EEVDF CPU scheduler for Linux," LWN.net, 2023. [Link](https://lwn.net/Articles/925371)

**Category:** Blog

**Summary:** The article explains EEVDF (Earliest Eligible Virtual Deadline First), which Peter Zijlstra proposed as an option alongside CFS (it was later merged in Linux 6.6). Each task gets a virtual deadline, and tasks that ask for shorter slices run sooner. It also describes "latency-nice," which separates how quickly a task gets the CPU from how much CPU it gets.

**Relevance:** SchedLab's SJF policy tries to do in user space what EEVDF does in the kernel: let short work go first. Our Priority policy is closer to nice values, which EEVDF turns into task weights. The article helps us explain which part of responsiveness the kernel already handles and which part (too many jobs at once) SchedLab adds.

### [7] B. Gregg, "Linux bcc/BPF Run Queue (Scheduler) Latency," brendangregg.com, 2016. [Link](https://brendangregg.com/blog/2016-10-08/linux-bcc-runqlat.html)

**Category:** Blog

**Summary:** Gregg defines run-queue latency as the time between a thread becoming runnable and actually running. He shows the `runqlat` tool, which prints this as a histogram. On a busy machine the histogram splits in two, with a second cluster in the tens of milliseconds caused by CPU saturation.

**Relevance:** This gives us a direct metric for "foreground responsiveness." We could use `runqlat` during our benchmarks to show that SchedLab reduces the slow cluster as N drops. It is also a candidate signal for the AIMD decrease step.

### [8] J. Evans, "Reasons to use your shell's job control," jvns.ca, 2024. [Link](https://jvns.ca/blog/2024/07/03/reasons-to-use-job-control/)

**Category:** Blog

**Summary:** A short, practical post on `&`, `Ctrl+Z`, `fg`, `bg`, `jobs`, `kill %N`, `disown`, and `wait`. It explains the three states a job can be in: foreground, background, and stopped.

**Relevance:** SchedLab adds a fourth state: queued (accepted but not started). This post lists the job-control commands users already expect, so we know which builtins (`jobs`, `fg`, `wait`) must also understand queued jobs.

---

## Community Discourse

### [9] Earnestly and G. Wooledge, "How does this wait -n work to cap parallelism?," bug-bash mailing list, 2019. [Link](https://lists.gnu.org/archive/html/bug-bash/2019-07/msg00195.html)

**Category:** Community

**Summary:** A user asks why a bash loop that calls `wait -n` once it has started N jobs keeps exactly N jobs running. Greg Wooledge's reply walks through it: the first N start at once, then each new job waits for any one job to finish. `wait -n` was added in bash 4.3.

**Relevance:** This is the common workaround for the problem SchedLab solves, written by hand in many scripts. It is our FIFO baseline, and it shows the limit lives in user scripts today instead of in the shell.

### [10] S. C. Dack, "Linux /proc/loadavg," bug-make mailing list, 2022. [Link](https://lists.gnu.org/archive/html/bug-make/2022-01/msg00035.html)

**Category:** Community

**Summary:** GNU make's `-l` option stops it from starting new jobs when the system load is too high. This thread is about make's use of the runnable-process count in `/proc/loadavg` on Linux, which a 2019 ChangeLog entry disabled because a load limit above the core count would let every job run. The author disagrees with clamping the limit, says the file behaves as documented on a normal Linux kernel, and proposes turning the feature off only on non-standard systems.

**Relevance:** Like our Adaptive policy, `make -l` decides whether to start a job based on a load signal. The thread shows that the signal and its limit matter: a limit set above what the signal can reach lets every job run. We should decide an upper bound for N in the Adaptive policy for the same reason.

---

## Source Code Analysis

### [11] M. Alcantu, "simple_shell: shell_launch()," GitHub, 2025. [Link](https://github.com/AlcantuMurilo77/simple_shell/blob/02354f6ebe2ea622cb4cf9e6cce6bc0022286f7e/shell.c#L15-L25)

**Category:** Source Code

**Summary:** `shell_launch` is the entire execution path of a minimalist shell. It calls the author's wrappers `Fork()`, `Execvp()`, and `Wait()` around the standard system calls: the child runs the command, the parent blocks until it finishes. There is no queue, no job table, and no background handling, just the bare fork/exec/wait pattern.

**Relevance:** SchedLab's `exec.c` runs foreground commands with the same fork, execvp, and wait pattern, so this function is a small version of what we already have. What we will add is a queue between parsing and forking, and `waitpid(WNOHANG)` in a SIGCHLD handler for background jobs, which this shell does not have. Reading it showed us which part of our code is the standard shell and which part is the extension.

### [12] MIT PDOS, "xv6: scheduler()," GitHub, 2020. [Link](https://github.com/mit-pdos/xv6-public/blob/eeb7b415dbcb12cc362d0783e41c3d1f44066b17/proc.c#L322-L356)

**Category:** Source Code

**Summary:** `scheduler()` is the xv6 round-robin scheduler. It loops forever over the process table. For each process in RUNNABLE state, it switches to that process's address space (`switchuvm`) and context-switches into it (`swtch`), then moves on to the next one, which gives round-robin scheduling.

**Relevance:** This is the same decision SchedLab's Policy Queue makes, but in user space: pick the next job to run when a slot opens. xv6 shows the minimal loop (scan, check state, switch). SchedLab replaces the scan with a min-heap keyed by policy (FIFO, SJF, nice), but the structure is the same: choose one runnable job, dispatch it, and repeat when a slot frees up.
