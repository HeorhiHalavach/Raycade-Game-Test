#include "Demo.hpp"
#include <cmath>

SCENE("start") {
    using namespace demo;
    Label(scene, {76, 70}, "RAYCADE", 18, cyan);
    Label(scene, {72, 112}, "AWARIA\nSTACJI", 76).LineHeight(0.95f);
    Button(scene, {76, 397}, 340, "Nowa gra", NewGame);
    Button(scene, {76, 463}, 340, "Ustawienia", [&engine] { engine.SwitchScene("ustawienia"); });
    Button(scene, {76, 529}, 340, "Zamknij", [&engine] { engine.Quit(); });
    auto& preview = scene.Spawn<node::visual::Sprite>()
                        .Sheet("kenney/characters.png", {24, 24})
                        .Animation("bieg", 2, 2, 6)
                        .Play("bieg")
                        .Anchor({0.5f, 1})
                        .Position({925, 430})
                        .Scale({7, 7});
    scene.Spawn<node::shapes::Rect>().Position({690, 437}).Size({470, 4}).Paint(cyan).Filled();
    scene.OnUpdate([&engine, &preview](float) {
        const auto& input = engine.Input();
        if (input.KeyPressed(KEY_ENTER)) NewGame();
        preview.Position({925, 430 + 4 * std::sin(static_cast<float>(engine.Time()) * 2)});
    });
}

SCENE("wynik") {
    using namespace demo;
    Label(scene, {96, 175}, "Stacja znów działa.", 62);
    auto& result = Label(scene, {100, 285}, "", 26, muted);
    Button(scene, {100, 485}, 300, "Zagraj ponownie", NewGame);
    Button(scene, {420, 485}, 200, "Menu", Menu);
    scene.OnUpdate([&result](float) {
        const auto& progress = ProgressOf();
        result.Content("Czas: {:.1f} s     Odrodzenia: {}", progress.seconds, progress.deaths);
    });
}
