# visual-novel-engine

This project uses C++26 modules with `import std;`, so it needs a recent Clang toolchain and the Ninja generator.

## NixOS / Linux

Requirements:

- Nix with `nix-command` and `flakes` enabled

Setup and build:

```sh
nix develop "path:."
cmake --preset clang-nix
cmake --build --preset clang-nix
```

Run the binary:

```sh
./build/visual-novel-engine
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
cmake --preset clang-homebrew
cmake --build --preset clang-homebrew
```

## LSP / clangd

- VS Code is already configured to use `scripts/run-clangd`
- Install the `llvm-vs-code-extensions.vscode-clangd` extension
- Run a configure step first so `build/compile_commands.json` exists
- If clangd shows stale diagnostics, restart clangd or reload the editor window

For other editors, point clangd at `scripts/run-clangd` and use `build/` as the compilation database directory.
