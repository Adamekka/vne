#include "parser/parser.hpp"
#include "raylib.h"
#include <print>

auto main(int32_t argc, char* argv[]) -> std::int32_t {
    if (argc < 2) {
        std::println("Usage: {} <project>", argv[0]);
        return 1;
    }

    parser::Parser parser{std::filesystem::path{argv[1]}};
    const auto script = parser.parse();

    InitWindow(800, 450, "Visual Novel Engine");

    while (!WindowShouldClose()) {
        BeginDrawing();
        EndDrawing();
    }

    CloseWindow();
}
