# visual-novel-engine

A C++/raylib visual novel engine that runs Markdown script packages.

The script format is defined in the [VNE specification v1](spec/v1/spec.md). Start there when writing scripts or changing parser/runtime behavior.

## Quick Start

Configure the project once, then run the bundled example:

```sh
cmake .
./run-example
```

`./run-example` builds the engine and runs `spec/v1/example` from that directory so the example's relative asset paths resolve correctly.

## Script Packages

A script package is a directory with a `main.md` entry point. Every script file starts with the version header:

```md
<!-- v1 -->
```

Useful references:

- [Specification](spec/v1/spec.md)
- [Full example](spec/v1/example/main.md)
- [Hello world example](spec/v1/hello-world/main.md)

Run a package or a single script file with:

```sh
./visual-novel-engine <project-path>
```

If the path is a directory, the engine loads `<project-path>/main.md`.

## Build

### NixOS / Linux

Requirements:

- Nix with `nix-command` and `flakes` enabled

Configure and build:

```sh
nix develop
cmake .
cmake --build .
```

### macOS

Install the toolchain:

```sh
brew install llvm cmake
```

Configure and build:

```sh
cmake .
cmake --build .
```

## Project Layout

- `src/parser`: Markdown script parsing and instruction model
- `src/engine`: runtime execution, dialogue, choices, audio, sprites, and backgrounds
- `spec/v1`: v1 script specification and examples

## Windows

Microslop OS not supported and never will be.
