#include "Demo.hpp"
#include "FusePanel.hpp"
#include <array>

namespace {
struct Wiring {
    int selected = -1;
    std::array<bool, 3> connected{};
    std::string message = "Połącz kolory";
};
constexpr std::array<Color, 3> colors{demo::cyan, demo::amber, Color{177, 140, 246, 255}};
constexpr std::array<int, 3> rows{2, 0, 1};
}

SCENE("naprawa") {
    using namespace demo;
    auto wires = std::make_shared<Wiring>();
    auto& title = Label(scene, {100, 90}, "1/2 · Obwody", 48);
    auto& status = Label(scene, {100, 185}, "", 21, muted);
    auto& contacts = scene.Spawn<node::Group>().ScreenSpace();
    auto& fuses = scene.Spawn<FusePanel>().ScreenSpace().Visible(false);
    for (int i = 0; i < 3; ++i) {
        contacts.Spawn<node::shapes::Circle>()
            .Position({300, 290.0f + i * 105})
            .Radius(24)
            .Paint(colors[i])
            .Filled()
            .OnClick([wires, i](auto&) {
                if (!wires->connected[i]) wires->selected = i;
            });
        contacts.Spawn<node::shapes::Circle>()
            .Position({980, 290.0f + rows[i] * 105})
            .Radius(24)
            .Paint(colors[i])
            .Filled()
            .OnClick([wires, i](auto&) {
                if (wires->connected[i]) return;
                if (wires->selected != i) {
                    wires->message = "Inny kolor";
                    return;
                }
                wires->connected[i] = true;
                wires->selected = -1;
                wires->message = "Połącz kolory";
                Engine::Get().Audio().Sound("click").Play();
            });
    }
    Button(scene, {100, 605}, 300, "Powrót do gry", [&engine] {
        engine.Audio().Bus("effects").Paused(false);
        engine.SwitchScene("stacja");
    });
    scene.OnUpdate([wires, &title, &status, &contacts, &fuses](float) {
        const bool wired = wires->connected == std::array<bool, 3>{true, true, true};
        contacts.Visible(!wired);
        fuses.Visible(wired);
        title.Content(wired ? "2/2 · Bezpieczniki" : "1/2 · Obwody");
        status.Content(wired ? fuses.Message() : wires->message);
    });
    scene.OnDraw([wires](PrimitiveRenderer& r) {
        if (wires->connected == std::array<bool, 3>{true, true, true}) return;
        r.Raster(Rasterizer::Custom).Filled(false);
        for (int i = 0; i < 3; ++i) {
            const Point2D from{300, 290.0f + i * 105};
            if (wires->connected[i])
                r.Paint(colors[i]).Polyline(
                    std::vector<Point2D>{
                        from,
                        {520.0f + i * 65, from.Y()},
                        {740.0f + i * 45, 290.0f + rows[i] * 105},
                        {980, 290.0f + rows[i] * 105}
                    }
                );
            else if (wires->selected == i)
                r.Paint(colors[i]).Line(from, Engine::Get().Input().MousePosition());
        }
    });
}
