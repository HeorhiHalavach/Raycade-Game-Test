#include "CargoPuzzle.hpp"
#include "Player.hpp"
#include "ReactorCore.hpp"
#include <algorithm>
#include <cmath>

namespace {
using namespace demo;

node::physics::StaticBody& Platform(
    Scene& scene,
    Vector2 position,
    Vector2 size,
    bool oneWay = false
) {
    auto& body = scene.Spawn<node::physics::StaticBody>()
                     .Position(position)
                     .CollisionLayer(worldLayer)
                     .CollisionMask(playerLayer | crateLayer);
    body.Spawn<node::collider::Rect>().Size(size).OneWay(oneWay);
    body.Spawn<node::shapes::Rect>().Size(size).Paint(border).Filled();
    body.Spawn<node::shapes::Rect>().Size({size.x, 4}).Paint(cyan).Filled();
    return body;
}

void Cell(Scene& scene, Vector2 position) {
    auto& cell = scene.Spawn<node::physics::Area>().Position(position).CollisionMask(playerLayer);
    cell.Spawn<node::collider::Circle>().Radius(21);
    auto& picture = Tile(cell, 67, {0, 0});
    picture.Anchor({0.5f, 0.5f});
    picture.OnUpdate([](auto& self, float dt) { self.Rotate(dt); });
    cell.OnBodyEnter([](auto& self, node::physics::PhysicsBody& body) {
        if (!dynamic_cast<Player*>(&body)) return;
        ++ProgressOf().cells;
        Engine::Get().Audio().Sound("cell").Play();
        self.Destroy();
    });
}

constexpr const char* reactorShader = R"(#version 330
in vec2 fragTexCoord;
in vec4 fragColor;
uniform sampler2D texture0;
uniform vec4 colDiffuse;
uniform float time;
out vec4 finalColor;
void main() {
    vec2 uv = fragTexCoord;
    uv.x += sin(uv.y * 28.0 + time * 3.0) * 0.025;
    vec4 color = texture(texture0, uv) * colDiffuse * fragColor;
    float bands = 0.75 + 0.25 * sin(uv.y * 50.0 - time * 6.0);
    finalColor = vec4(color.rgb * bands, color.a);
})";
}

SCENE("stacja") {
    using namespace demo;
    auto& progress = ProgressOf();
    progress = {};
    engine.Audio().Bus("effects").Paused(false);
    scene.Gravity({0, 1400});

    scene.OnDrawWorld([](PrimitiveRenderer& r) {
        r.Paint(background).Filled().Rect({-800, -600}, {4600, 1600});
        for (int x = -800; x < 3500; x += 80)
            r.Paint(panel).Filled(false).Rect({static_cast<float>(x), -200}, {80, 820});
        r.Paint(border).Filled().Rect({-800, 310}, {4600, 14});
        for (int x = 0; x < 3000; x += 300)
            r.Paint(border).Filled().Rect({static_cast<float>(x), 240}, {12, 350});
    });
    Platform(scene, {-80, 620}, {1280, 120});
    Platform(scene, {1410, 620}, {1800, 120});
    Platform(scene, {-80, 0}, {20, 700});
    Platform(scene, {3040, 0}, {20, 700});
    Platform(scene, {300, 510}, {280, 18}, true);
    Platform(scene, {330, 400}, {160, 18}, true)
        .OnFixedUpdate([time = 0.0f](auto& self, float dt) mutable {
            time += dt;
            self.Position({330 + 55 * std::sin(time * 0.6f), 400});
        });
    Platform(scene, {1000, 520}, {180, 18}, true);
    Platform(scene, {1460, 430}, {160, 18}, true);
    Platform(scene, {1640, 520}, {100, 18}, true);
    Platform(scene, {1200, 455}, {180, 18}, true)
        .OnFixedUpdate([time = 0.0f](auto& self, float dt) mutable {
            time += dt;
            self.Position({1200 + 40 * std::sin(time * 0.9f), 455});
        });

    auto& gate = Platform(scene, {770, 420}, {24, 200});
    AddCargoPuzzle(scene, gate);
    Cell(scene, {550, 460});
    Cell(scene, {1540, 390});

    auto& player = scene.Spawn<Player>().Position({100, 600});
    scene.Camera().Offset({640, 360}).Target({640, 360});
    auto& checkpoint =
        scene.Spawn<node::physics::Area>().Position({1650, 540}).CollisionMask(playerLayer);
    checkpoint.Spawn<node::collider::Rect>().Size({70, 80});
    Tile(checkpoint, 111, {10, 40});
    checkpoint.OnBodyEnter([](auto&, node::physics::PhysicsBody& body) {
        if (auto* actor = dynamic_cast<Player*>(&body)) actor->checkpoint = {1690, 600};
    });

    auto& console = scene.Spawn<node::Group>().Position({1860, 556});
    Tile(console, 130, {0, 0}, 3);
    console.Spawn<node::shapes::Circle>().Position({27, 14}).Radius(5).Paint(cyan).Filled();
    auto shader = Shared(ShaderProgram::FromCode("", reactorShader));
    auto& reactor = scene.Spawn<node::visual::RenderGroup>()
                        .Position({2350, 400})
                        .Size({160, 220})
                        .Shader(shader);
    AddReactorCore(reactor);
    scene.Spawn<node::sound::Emitter>()
        .Position({2430, 500})
        .Sound("reactor")
        .MaxDistance(700)
        .Play();
    auto& exit =
        scene.Spawn<node::physics::Area>().Position({2860, 530}).CollisionMask(playerLayer);
    exit.Spawn<node::collider::Rect>().Size({80, 90});
    Tile(exit, 150, {5, 90}, 4).Anchor({0, 1});
    exit.OnBodyEnter([&engine](auto&, node::physics::PhysicsBody& body) {
        auto& state = ProgressOf();
        if (dynamic_cast<Player*>(&body) && state.cells == 2 && state.repaired) {
            engine.SwitchScene("wynik").UnloadScene("naprawa").UnloadScene("stacja");
        }
    });

    scene.Spawn<node::visual::Text>()
        .Position({1840, 520})
        .Content("E · Panel")
        .FontSize(20)
        .Paint(cyan);
    auto& hud = Label(scene, {32, 26}, "", 22);
    auto& objective = Label(scene, {32, 65}, "", 19, cyan);
    Label(scene, {32, 680}, "A/D · ruch    Spacja · skok    Esc · menu", 17, muted);
    scene.OnUpdate([&engine, &scene, &player, &hud, &objective, &reactor, shader](float dt) {
        auto& state = ProgressOf();
        state.seconds += dt;
        const auto& input = engine.Input();
        if (input.KeyPressed(KEY_R)) {
            NewGame();
            return;
        }
        if (input.KeyPressed(KEY_F1)) scene.DebugColliders(!scene.IsDebugColliders());
        if (input.KeyPressed(KEY_F2)) reactor.Shader(reactor.Shader() ? nullptr : shader);
        if (shader) shader->Uniform("time", static_cast<float>(engine.Time()));
        reactor.Paint(state.repaired && state.cells == 2 ? WHITE : Color{185, 205, 220, 255});
        const Vector2 position = player.Position();
        const float targetX = std::clamp(position.x, 640.0f, 2400.0f);
        const float cameraX = scene.Camera().Target().x;
        scene.Camera().Target({cameraX + (targetX - cameraX) * (1 - std::exp(-7 * dt)), 360});
        if (input.KeyPressed(KEY_E) && std::hypot(position.x - 1887, position.y - 600) < 110) {
            engine.Audio().Bus("effects").Paused();
            engine.SwitchScene("naprawa");
        }
        hud.Content("Moduły: {}/2", state.cells);
        objective.Content(
            !state.gate       ? "3 kg na wadze"
            : state.cells < 2 ? "Zbierz moduły"
            : !state.repaired ? "Napraw panel"
                              : "Dotrzyj do wyjścia"
        );
    });
}
