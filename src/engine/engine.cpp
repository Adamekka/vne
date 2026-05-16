#include "engine.hpp"
#include "audio_manager.hpp"
#include "background_manager.hpp"
#include "choice_manager.hpp"
#include "dialogue_manager.hpp"
#include "sprite_manager.hpp"

namespace engine {

Engine::Engine(parser::ParsedScript script)
    : interpreter{*this, std::move(script)} {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(800, 600, "Visual Novel Engine");

    SetTargetFPS(60);

    this->interpreter.next();
}

Engine::~Engine() {
    CloseWindow();
}

auto Engine::run() -> void {
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        AudioManager::instance().update();
        BackgroundManager::instance().update();
        SpriteManager::instance().update();
        DialogueManager::instance().update();
        const auto selected_choice{ChoiceManager::instance().update()};

        if (selected_choice.has_value()) {
            this->interpreter.select_choice(selected_choice.value());
        } else if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            this->interpreter.next();
        }

        EndDrawing();
    }
}

} // namespace engine
