/**
 * @file StartScene.cpp
 * @brief Scena startowa - punkt wyjścia do własnej gry.
 */

#include <raycade>

/** @brief Konfiguruje scenę startową. */
SCENE("start") {
    const Vector2 view = engine.ViewSize();

    scene.Spawn<node::visual::Text>()
        .Content("Raycade - nowy projekt")
        .FontSize(48)
        .Paint(DARKGRAY)
        .Anchor({0.5f, 0.5f})
        .Position({view.x / 2, view.y / 2 - 60});

    scene.Spawn<node::shapes::Circle>()
        .Position({view.x / 2, view.y / 2 + 40})
        .Radius(30)
        .Paint(ORANGE)
        .Filled()
        .OnClick([](node::shapes::Circle& self) {
            self.Paint(::ColorFromHSV(Engine::Get().RandomFloat(0, 360), 0.8f, 0.9f));
        });
}
