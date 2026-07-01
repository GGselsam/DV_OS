# DV OS

**DV OS** is my 64-bit operating system built from scratch in C and assembly.

## 🚀 Features

- BIOS bootloader (16-bit → 64-bit long mode)
- A20 enable, page table setup (PML4, PDPT, PDT, PT)
- VGA text output with colors
- CPU detection (SSE, SSE2, SSE3)
- 64-bit long mode operation

## 🛠️ Build

### Requirements

- NASM
- GCC (x86_64)
- LD
- QEMU

- ## 📌 Project rules

- **Architecture:** x86_64 only
- **Language:** English only (code, comments, commits)
- **Comments:** Profanity is allowed and welcome. Fuck yeah.
- **Allowed languages:** C, Assembly, Rust, Bash, Python (only if Bash can't handle it), Makefile
- **Banned languages:** C++, Zig, and others

### Commands

```bash
make build   # Build the OS
make run     # Run in QEMU
make clean   # Clean build files
make help    # Show all commands
