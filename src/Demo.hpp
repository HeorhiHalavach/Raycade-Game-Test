#pragma once
#include <functional>
#include <raycade>
#include <string>

namespace demo {
inline constexpr unsigned worldLayer = 1, playerLayer = 2, crateLayer = 4;

inline constexpr Color background{13, 21, 33, 255}, panel{24, 37, 53, 255};
inline constexpr Color border{46, 65, 84, 255}, ink{231, 239, 246, 255};
inline constexpr Color muted{147, 169, 188, 255}, cyan{71, 220, 213, 255}, amber{255, 186, 92, 255};

struct Progress {
    int cells = 0, deaths = 0;
    float seconds = 0;
    bool gate = false, repaired = false;
};
inline Progress& ProgressOf() {
    return *Engine::Get().Context<Progress>();
}

inline node::visual::Text& Label(
    Scene& scene,
    Vector2 position,
    std::string text,
    float size = 20,
    Color color = ink
) {
    return scene.Spawn<node::visual::Text>()
        .Position(position)
        .Content(std::move(text))
        .FontSize(size)
        .Paint(color)
        .ScreenSpace();
}
inline node::shapes::Rect& Button(
    Scene& scene,
    Vector2 position,
    float width,
    std::string text,
    std::function<void()> action
) {
    auto& button = scene.Spawn<node::shapes::Rect>()
                       .Position(position)
                       .Size({width, 54})
                       .Paint(panel)
                       .Filled()
                       .ScreenSpace();
    button.Spawn<node::visual::Text>()
        .Position({18, 15})
        .Content(std::move(text))
        .FontSize(21)
        .Paint(ink);
    button.OnMouseEnter([](auto& self) { self.Paint(border); });
    button.OnMouseLeave([](auto& self) { self.Paint(panel); });
    button.OnClick([fn = std::move(action)](auto&) {
        Engine::Get().Audio().Sound("click").Play();
        fn();
    });
    return button;
}
inline node::visual::Sprite& Tile(Entity& parent, int index, Vector2 position, float scale = 2) {
    return parent.Spawn<node::visual::Sprite>()
        .Sheet("kenney/platformer.png", {18, 18})
        .Frame(index)
        .Pause()
        .Position(position)
        .Scale({scale, scale});
}
inline void NewGame() {
    auto& engine = Engine::Get();
    engine.Audio().Bus("effects").Paused(false);
    engine.UnloadScene("naprawa");
    if (engine.IsSceneLoaded("stacja")) engine.ReloadScene("stacja");
    engine.SwitchScene("stacja");
}
inline void Menu() {
    auto& engine = Engine::Get();
    engine.Audio().Bus("effects").Paused(false);
    engine.SwitchScene("start").UnloadScene("naprawa").UnloadScene("stacja");
}
}
