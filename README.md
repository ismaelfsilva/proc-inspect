# proc-inspect

A small, **read-only** Windows process / module / memory-region inspector written in C++17 using the Win32 API (Toolhelp + `VirtualQueryEx`).

It is intended as legitimate sysadmin / reverse-engineering tooling: it only **queries and lists** information. It does **not** read, write, inject, or modify any process memory.

## Features

- List running processes (PID, name).
- List loaded modules of a process (name, base address, size).
- List committed/reserved memory regions of a process (base address, size, state, protection) via `VirtualQueryEx`.

## Build

Requires CMake and a C++17 compiler.

```sh
cmake -B build -G Ninja -DCMAKE_CXX_COMPILER=g++ -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure
```

On MSVC the default generator also works:

```sh
cmake -B build
cmake --build build --config Release
ctest --test-dir build -C Release --output-on-failure
```

## Usage

```sh
proc-inspect ps              # list all processes
proc-inspect mods [pid]      # list modules of <pid> (default: current process)
proc-inspect mem  [pid]      # list memory regions of <pid> (default: current process)
```

## License

MIT — see [LICENSE](LICENSE).
