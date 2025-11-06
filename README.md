# SYSC4001_A1_P2  
**Authors:**  
- Mohamed Cherif Bah 101292844
- Nathaniel Baird 101314576

---

## Overview
This repository contains all five parts of Assignment 2, Part 2 (SYSC4001).  
Each part demonstrates a different concept of process management in Linux using C.

An additional **images/** folder contains screenshots showing each program running correctly.

---

##Parts Summary

| Part | Description | File(s) |
|------|--------------|---------|
| **1** | Using `fork()` to create two concurrent processes that run indefinitely and increment counters. | `part1_fork_101292844_101314576.c` |
| **2** | Using `exec()` to start a second process (separate executable). Process 1 increments, Process 2 decrements. | `part2_process1_101292844_101314576.c`, `part2_process2_101292844_101314576.c` |
| **3** | Adding `wait()` so Process 1 waits for Process 2 to finish when its value < -500. | `part3_wait_101292844_101314576.c` |
| **4** | Introducing **shared memory** (`shmget`, `shmat`, `shmdt`, `shmctl`). Both processes share two variables (`multiple`, `counter`). | `part4_sharedmem_101292844_101314576.c` |
| **5** | Adding **semaphores** (`semget`, `semop`, `semctl`) to protect concurrent access to shared memory. | `process1_101292844_101314576.c`, `process2_101292844_101314576.c` |

---

##How to Compile and Run

Each `.c` file can be compiled using **gcc** in the terminal.

### Example (Part 5 – final version)
```bash
gcc -Wall -O2 process2_101292844_101314576.c -o process2
gcc -Wall -O2 process1_101292844_101314576.c -o process1
./process1
