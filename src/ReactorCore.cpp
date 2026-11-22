#include "ReactorCore.hpp"
#include <cmath>

namespace {
constexpr Vector2 center{80, 110}, radius{54, 86};
constexpr unsigned reactorLayer = 8;
constexpr int wallSegments = 32, ballCount = 8;
constexpr float ballSpeed = 90;

Vector2 Boundary(float angle) {
    return {center.x + radius.x * std::cos(angle), center.y + radius.y * std::sin(angle)};
}
}

namespace demo {
void AddReactorCore(node::visual::RenderGroup& reactor) {
    const auto shell = [&reactor](Vector2 size, Color color) -> node::shapes::Ellipse& {
        return reactor.Spawn<node::shapes::Ellipse>()
            .Position(center)
            .Radius(size)
            .Paint(color)
            .Filled();
    };
    auto& halo = shell({71, 103}, Color{71, 220, 213, 18});
    halo.OnUpdate([time = 0.0f](auto& self, float dt) mutable {
        time += dt;
        self.Paint(Color{71, 220, 213, static_cast<unsigned char>(18 + 6 * std::sin(time * 2))});
    });
    shell({68, 100}, Color{71, 220, 213, 35});
    shell({65, 97}, Color{76, 100, 119, 255});
    shell({61, 93}, Color{24, 40, 56, 255});
    shell({57, 89}, Color{53, 172, 177, 255});
    shell(radius, Color{9, 28, 41, 255}).Raster(Rasterizer::Custom);
    shell({3, 27}, Color{159, 244, 246, 28}).Position({42, 98});
    shell({2, 12}, Color{159, 244, 246, 55}).Position({49, 60});

    for (float x : {8.0f, 136.0f}) {
        for (float y : {67.0f, 143.0f}) {
            reactor.Spawn<node::shapes::Rect>()
                .Position({x, y})
                .Size({16, 14})
                .Paint(Color{48, 66, 82, 255})
                .Filled();
            reactor.Spawn<node::shapes::Circle>()
                .Position({x + 8, y + 7})
                .Radius(2)
                .Paint(Color{137, 161, 180, 255})
                .Filled();
        }
    }
    reactor.Spawn<node::shapes::Rect>()
        .Position({56, 8})
        .Size({48, 12})
        .Paint(Color{58, 81, 99, 255})
        .Filled();
    reactor.Spawn<node::shapes::Rect>()
        .Position({36, 204})
        .Size({88, 12})
        .Paint(Color{48, 66, 82, 255})
        .Filled();
    reactor.Spawn<node::shapes::Rect>()
        .Position({26, 216})
        .Size({108, 4})
        .Paint(Color{76, 100, 119, 255})
        .Filled();
    reactor.Spawn<node::shapes::Rect>().Position({59, 208}).Size({42, 3}).Paint(cyan).Filled();

    auto& wall = reactor.Spawn<node::physics::StaticBody>()
                     .CollisionLayer(reactorLayer)
                     .CollisionMask(reactorLayer)
                     .Friction(0)
                     .Bounce(1);
    for (int i = 0; i < wallSegments; ++i) {
        wall.Spawn<node::collider::Line>()
            .From(Boundary(2 * PI * i / wallSegments))
            .To(Boundary(2 * PI * (i + 1) / wallSegments));
    }
    for (int i = 0; i < ballCount; ++i) {
        const float angle = 0.3f + i * 0.9f;
        const Vector2 initialVelocity{ballSpeed * std::cos(angle), ballSpeed * std::sin(angle)};
        auto& ball = reactor.Spawn<node::physics::RigidBody>()
                         .Position({center.x + (i % 2 ? 20.0f : -20.0f), 65.0f + (i / 2) * 30})
                         .GravityScale(0)
                         .Friction(0)
                         .Bounce(1)
                         .RotationLocked()
                         .CollisionLayer(reactorLayer)
                         .CollisionMask(reactorLayer)
                         .Velocity(initialVelocity);
        const Color color = i % 2 ? amber : ink;
        ball.Spawn<node::shapes::Circle>()
            .Radius(7)
            .Paint(Color{color.r, color.g, color.b, 22})
            .Filled();
        ball.Spawn<node::collider::Circle>().Radius(4).Paint(color).Filled().Visible().Raster(
            Rasterizer::Custom
        );
        ball.Spawn<node::shapes::Circle>().Position({-1, -1}).Radius(1).Paint(WHITE).Filled();
        ball.OnFixedUpdate([initialVelocity](auto& self, float) {
            const Vector2 velocity = self.Velocity();
            const float speed = std::hypot(velocity.x, velocity.y);
            self.Velocity(
                speed > 0.01f
                    ? Vector2{velocity.x * ballSpeed / speed, velocity.y * ballSpeed / speed}
                    : initialVelocity
            );
        });
    }
}
}
