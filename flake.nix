{
    description = "Visual novel engine development shell";

    inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";

    outputs = { nixpkgs, ... }:
        let
            systems = [
                "x86_64-linux"
                "aarch64-linux"
            ];
            forAllSystems = f:
                nixpkgs.lib.genAttrs systems (system:
                    let
                        pkgs = import nixpkgs { inherit system; };
                        llvm = pkgs.llvmPackages_22;
                        compilerRtLibc = llvm."compiler-rt-libc";
                        commonCxxFlags = pkgs.lib.concatStringsSep " " [
                            "-idirafter ${pkgs.stdenv.cc.libc.dev}/include"
                            "-isystem ${llvm.libcxx.dev}/include"
                            "-isystem ${compilerRtLibc.dev}/include"
                            "-isystem ${llvm.libcxx.dev}/include/c++/v1"
                            "-D_LIBCPP_HARDENING_MODE=_LIBCPP_HARDENING_MODE_EXTENSIVE"
                        ];
                        commonLdFlags = pkgs.lib.concatStringsSep " " [
                            "-L${llvm.libcxx}/lib"
                            "-Wl,-rpath,${llvm.libcxx}/lib"
                        ];
                    in
                    f {
                        inherit commonCxxFlags commonLdFlags;
                        inherit pkgs system;
                        inherit llvm;
                    }
                );
        in {
            devShells = forAllSystems ({ pkgs, commonCxxFlags, commonLdFlags, llvm, ... }: {
                default = pkgs.mkShell {
                    hardeningDisable = [
                        "fortify"
                        "fortify3"
                    ];

                    packages = [
                        llvm.libcxxClang
                        pkgs.cmake
                        pkgs.ninja
                        llvm.libcxx
                        llvm.clang-tools
                    ];

                    CXXFLAGS = commonCxxFlags;
                    LDFLAGS = commonLdFlags;
                };
            });

            packages = forAllSystems ({ pkgs, commonCxxFlags, commonLdFlags, llvm, ... }: {
                default = llvm.libcxxStdenv.mkDerivation {
                    pname = "visual-novel-engine";
                    version = "0.1.0";
                    src = pkgs.lib.cleanSourceWith {
                        src = ./.;
                        filter = path: type:
                            let
                                baseName = builtins.baseNameOf path;
                            in
                            !(
                                baseName == ".direnv"
                                || baseName == ".git"
                                || baseName == "CMakeCache.txt"
                                || baseName == "CMakeFiles"
                                || baseName == "build"
                                || pkgs.lib.hasPrefix "build-" baseName
                                || baseName == "compile_commands.json"
                                || baseName == "result"
                            );
                    };

                    hardeningDisable = [
                        "fortify"
                        "fortify3"
                    ];

                    nativeBuildInputs = [
                        llvm.libcxxClang
                        pkgs.cmake
                        pkgs.ninja
                        llvm.libcxx
                    ];

                    CXXFLAGS = commonCxxFlags;
                    LDFLAGS = commonLdFlags;

                    cmakeGenerator = "Ninja";
                    cmakeFlags = [
                        "-DCMAKE_CXX_COMPILER=${llvm.libcxxClang}/bin/clang++"
                        "-DCMAKE_CXX_STANDARD_LIBRARY=libc++"
                    ];
                };
            });
        };
}
