# visual-novel-engine

This project requires a recent Clang toolchain.

## NixOS / Linux

Requirements:

- Nix with `nix-command` and `flakes` enabled

Setup and build:

```sh
nix develop "path:."
cmake -B build -G Ninja
cmake --build build
```

Run the binary:

```sh
./visual-novel-engine
```

Build the package through Nix:

```sh
nix build "path:."
./result/bin/visual-novel-engine
```

## macOS

Install the toolchain:

```sh
brew install llvm cmake ninja
```

Configure and build:

```sh
cmake -B build -G Ninja \
  -DCMAKE_C_COMPILER=/opt/homebrew/opt/llvm/bin/clang \
  -DCMAKE_CXX_COMPILER=/opt/homebrew/opt/llvm/bin/clang++ \
  -DCMAKE_CXX_STANDARD_LIBRARY=libc++ \
  -DCMAKE_OSX_SYSROOT=macosx
cmake --build build
```

## Windows

Goyslop OS not supported and never will be.

## LSP / clangd

- VS Code is already configured to use `scripts/run-clangd`
- Install the `llvm-vs-code-extensions.vscode-clangd` extension
- Run a configure step first so the repo-root `compile_commands.json` symlink exists
- If clangd shows stale diagnostics, restart clangd or reload the editor window

For other editors, point clangd at `scripts/run-clangd`, or run clangd from the repo root so it picks up `compile_commands.json` automatically.
