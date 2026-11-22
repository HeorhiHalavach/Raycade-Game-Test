#include "Demo.hpp"
#include <algorithm>

namespace {
void VolumeSlider(Scene& scene, Vector2 position, std::string name, audio::Bus& bus) {
    using namespace demo;
    Label(scene, {position.x, position.y - 32}, std::move(name), 21);
    auto& track = scene.Spawn<node::shapes::Rect>()
                      .Position(position)
                      .Size({360, 30})
                      .Paint(panel)
                      .Filled()
                      .ScreenSpace();
    auto& fill =
        track.Spawn<node::shapes::Rect>().Position({0, 12}).Size({0, 6}).Paint(cyan).Filled();
    auto& knob =
        track.Spawn<node::shapes::Circle>().Position({0, 15}).Radius(9).Paint(ink).Filled();
    auto& value = Label(scene, {position.x + 385, position.y + 3}, "", 20, muted);
    const auto change = [&bus](auto& self) {
        const Vector2 local = self.ScreenToLocal(Engine::Get().Input().MousePosition());
        bus.Volume(std::clamp(local.x / 360.0f, 0.0f, 1.0f));
    };
    track.OnClick(change);
    track.OnDrag([change](auto& self, Vector2) { change(self); });
    track.OnDragEnd([](auto&) { Engine::Get().Audio().Sound("click").Play(); });
    track.OnUpdate([&bus, &fill, &knob, &value](auto&, float) {
        const float gain = bus.Volume();
        fill.Size({360 * gain, 6});
        knob.Position({360 * gain, 15});
        value.Content("{:.0f}%", gain * 100);
    });
}

void Toggle(
    Scene& scene,
    Vector2 position,
    std::string name,
    std::function<bool()> read,
    std::function<void()> change
) {
    auto& button = demo::Button(scene, position, 340, std::move(name), std::move(change));
    auto& track = button.Spawn<node::shapes::Rect>()
                      .Position({270, 17})
                      .Size({52, 20})
                      .Paint(demo::border)
                      .Filled();
    track.Spawn<node::shapes::Circle>().Radius(8).Filled().OnUpdate(
        [read = std::move(read)](auto& self, float) {
            const bool enabled = read();
            self.Position({enabled ? 42.0f : 10.0f, 10}).Paint(enabled ? demo::cyan : demo::muted);
        }
    );
}
}

SCENE("ustawienia") {
    using namespace demo;
    Label(scene, {100, 70}, "Ustawienia", 48);
    Label(scene, {100, 170}, "Dźwięk", 26, cyan);
    VolumeSlider(scene, {100, 260}, "Ogólny", engine.Audio().Bus());
    VolumeSlider(scene, {100, 350}, "Muzyka", engine.Audio().Bus("music"));
    VolumeSlider(scene, {100, 440}, "Efekty", engine.Audio().Bus("effects"));
    Label(scene, {820, 170}, "Obraz", 26, cyan);
    Toggle(
        scene, {820, 225}, "Pełny ekran", [&engine] { return engine.IsFullscreen(); },
        [&engine] { engine.Fullscreen(!engine.IsFullscreen()); }
    );
    Toggle(
        scene, {820, 295}, "VSync", [&engine] { return engine.IsVSync(); },
        [&engine] { engine.VSync(!engine.IsVSync()); }
    );
    Label(
        scene, {100, 520},
        "A/D ruch · Spacja skok · E panel\nR restart · F1 kolizje · F2 shader · F11 pełny ekran",
        18, muted
    );
    Button(scene, {100, 620}, 220, "Powrót", [&engine] { engine.SwitchScene("start"); });
}
