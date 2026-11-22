#include "CargoPuzzle.hpp"
#include <algorithm>
#include <array>
#include <cmath>

namespace {
constexpr float scaleWidth = 230;
constexpr std::array<Vector2, 2> crateSpawns{Vector2{500, 488}, {410, 378}};
}

namespace demo {
void AddCargoPuzzle(Scene& scene, node::physics::StaticBody& gate) {
    for (int i = 0; i < 2; ++i) {
        const Vector2 start = crateSpawns[i];
        auto& crate = scene.Spawn<node::physics::RigidBody>()
                          .Position(start)
                          .Mass(static_cast<float>(i + 1))
                          .Friction(0.65f)
                          .CollisionLayer(crateLayer)
                          .CollisionMask(worldLayer | playerLayer | crateLayer);
        crate.Spawn<node::collider::Rect>().Position({-22, -22}).Size({44, 44});
        Tile(crate, 26, {-22, -22}, 44.0f / 18);
        crate.OnUpdate([start](auto& self, float) {
            if (self.Position().y > 900)
                self.Position(start).Velocity({0, 0}).AngularVelocity(0).Rotation(0);
        });
    }

    auto& scale = scene.Spawn<node::physics::Area>().Position({540, 525}).CollisionMask(crateLayer);
    scale.Spawn<node::collider::Rect>().Size({scaleWidth, 95});
    auto& indicator = scale.Spawn<node::shapes::Rect>()
                          .Position({0, 89})
                          .Size({scaleWidth, 6})
                          .Paint(amber)
                          .Filled();
    scale.Spawn<node::shapes::Rect>()
        .Position({0, -40})
        .Size({scaleWidth, 8})
        .Paint(border)
        .Filled();
    auto& chargeBar =
        scale.Spawn<node::shapes::Rect>().Position({0, -40}).Size({0, 8}).Paint(cyan).Filled();

    scale.OnFixedUpdate([&indicator, &chargeBar, charge = 0.0f](auto& self, float dt) mutable {
        float weight = 0;
        bool settled = true;
        for (auto* body : self.OverlappingBodies()) {
            if (const auto* crate = dynamic_cast<node::physics::RigidBody*>(body)) {
                const float x = crate->Position().x - self.Position().x;

                if (x < 21 || x > scaleWidth - 21) continue;
                weight += crate->Mass();
                const Vector2 velocity = crate->Velocity();
                settled &= std::hypot(velocity.x, velocity.y) < 12;
            }
        }

        const bool correct = std::fabs(weight - 3.0f) < 0.01f && settled;
        if (!ProgressOf().gate) {
            charge = correct ? std::min(charge + dt, 1.5f) : 0.0f;
            if (charge >= 1.5f) {
                ProgressOf().gate = true;
                Engine::Get().Audio().Sound("cell").Play();
            }
        }
        indicator.Paint(correct || ProgressOf().gate ? cyan : amber);
        chargeBar.Size({scaleWidth * charge / 1.5f, 8});
    });
    gate.OnFixedUpdate([](auto& self, float dt) {
        if (ProgressOf().gate) self.Position({770, std::max(150.0f, self.Position().y - 180 * dt)});
    });
}
}
