
# xv6 Strace Implementation

This project adds system tracing (`strace`) functionality to xv6 for debugging and analyzing system calls.

## Overview

### Key Features
- **Strace** can trace system calls, output recent traces, and filter traces based on success/failure or specific system calls.
- Added system calls to enable/disable tracing and dump trace logs.
- Supports race condition debugging and process testing.

---

## Files and Modifications

### `proc.h`
1. Added `strace_flag` and `strace_option` fields in the `proc` struct.
2. Included `strace.h`.

### `sysproc.c`
1. Added `sys_trace()`.
2. Added `sys_strace_dump()`.
3. Added `strcmp()` logic for `sys_strace_dump`.
4. Added `sys_strace_option()`.
5. Included `strace.h`.

### `proc.c`
1. Added initializations for `strace_flag` and `strace_option` in `allocproc()`.
2. Added inheritance of strace_flag and strace_options logic in fork().
3. Added exit logic for `strace_option` in `exit()`.
4. Added “#include strace.h”.

### `race.c`
- Added to test `strace`'s usefulness in debugging race conditions.

### `strace.c`
- Created in xv6 to parse `strace` command-line inputs.

### `strace.h`
- Created in xv6 to hold all `strace` definitions.

### `test.c`
- Added to test child processes using `strace`.

### `syscall.c`
1. Included `strace.h`.
2. Added buffer definitions.
3. Added `log_strace_event()`.
4. Added strace output logic in `syscall()`.
5. Added new system calls to extern and syscall array.
6. Created a syscall names array.

### `user.h`
- Added system calls.

### `syscall.h`
- Added system call numbers.

### `usys.S`
- Added system calls.

### `Makefile`
1. Added `_race\` to `UPROGS`.
2. Added `_test\` to `UPROGS`.
3. Added `_strace\` to `UPROGS`.

---

## System Calls

### `strace(int)`
- Turns tracing **on/off** for a process.

### `strace_dump()`
- Outputs the N most recent traced system calls.

### `strace_option(int, int, int)`
- Filters tracing:
    - `-e`: Takes in a value for the -e filter (-1 if no -e filter)
    - `-s`: Takes in a 0 or 1 for the -s filter
    - `-f`: Takes in a 0 or 1 for the -f filter
    - Can’t be used outside of strace on/off bounds

---

## Usage Instructions

### Command Line Options

1. **`strace on`**
   - Enables tracing for all processes.

2. **`strace off`**
   - Disables tracing.

3. **`strace run command [args]`**
   - Runs a specific command with tracing enabled.
   - Can be used inside of strace on/off or outside

4. **`strace dump`**
   - Outputs the N most recent traces.
   - Can be used inside of strace on/off or outside

5. **`strace -e <syscall_name>`**
   - Filters trace to a specific system call.
   - Can only be used inside of strace on/off, will return error message if used outside

6. **`strace -s`**
   - Filters successful calls only.
   - Can only be used inside of strace on/off, will return error message if used outside

7. **`strace -f`**
   - Filters failed calls only.
   - Can only be used inside of strace on/off, will return error message if used outside

8. **Combined Options**
   - `strace -s -e <syscall_name>`
   - `strace -f -e <syscall_name>`
   - Can only be used inside of strace on/off, will return error message if used outside
---

## Warnings
- Options cannot be used in the same line as `strace on/off`.
- Filters (`-e`, `-s`, `-f`) **must** be used after `strace on`.
- Incorrect use will display an error message with valid usage options.
- Strace on cannot be combined with a command; use strace on and then type the command for the next input

---
## Added Programs

1. **`Test.c`**
   - Prints messages, forks processes, and attempts invalid file opens.
   - Example output:
     ```
     Hello world!
     hello
     hello
     hello
     ```

2. **`Race.c`**
   - Debugs race conditions in xv6.
   - Expected outputs are integers less than or equal to 4.
   - Can call strace to log system calls.

3. **`app.c` in Linux**
   - Compile using `gcc`:
     ```
     gcc app.c -o app
     ```
   - Run multiple times to observe race conditions.
   - Expected outputs are integers less than or equal to 4.



## How to Run

1. **Build xv6**:
   ```bash
   make
   ```

2. **Run in QEMU**:
   ```bash
   make qemu
   ```


