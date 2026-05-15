{
  description = "C++ Flake";

  inputs = {
    flake-parts.url = "github:hercules-ci/flake-parts";
    nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  };

  outputs = inputs@{ flake-parts, nixpkgs, ... }:
    flake-parts.lib.mkFlake { inherit inputs; } {
      systems = [ "x86_64-linux" "aarch64-linux" "aarch64-darwin" "x86_64-darwin" ];
      perSystem =
        { pkgs, ... }: {
          devShells.default = pkgs.mkShell.override { stdenv = pkgs.clangStdenv; } {
            packages = with pkgs; [
              alsa-lib
              clang-analyzer
              clang-tools
              cmake
              libGL
              libpulseaudio
              libx11
              libxcursor
              libxext
              libxi
              libxinerama
              libxrandr
            ];

            shellHook = ''
              # libglvnd and miniaudio load graphics/audio backends at runtime.
              export LD_LIBRARY_PATH="${pkgs.lib.makeLibraryPath (with pkgs; [ alsa-lib libpulseaudio ])}:/run/opengl-driver/lib''${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"

              echo 'Make sure `programs.nix-ld.enable` is set to true for LLDB to work.';
            '';
          };
        };
    };
}
