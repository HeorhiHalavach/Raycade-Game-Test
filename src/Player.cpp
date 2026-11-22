#include "Player.hpp"
#include <algorithm>
#include <cmath>

namespace demo {
Player::Player() {
    static_cast<Drawable&>(*this).Layer(1);
    CollisionLayer(playerLayer).CollisionMask(worldLayer | crateLayer);
    Spawn<node::collider::Rect>().Position({-12, -38}).Size({24, 38});
    sprite_ = &Spawn<node::visual::Sprite>()
                   .Sheet("kenney/characters.png", {24, 24})
                   .Animation("stoi", 2, 1, 1)
                   .Animation("bieg", 2, 2, 9)
                   .Animation("skok", 3, 1, 1)
                   .Anchor({0.5f, 1})
                   .Scale({2, 2})
                   .Play("stoi");
    auto sheet = std::make_shared<SpriteSheet>(*sprite_->Sheet());
    sheet->Event("bieg", 0, "krok");
    sprite_->Sheet(sheet).OnFrameEvent([](auto&, const std::string&) {
        Engine::Get().Audio().Sound("step").Play();
    });
    Spawn<node::sound::Listener>();
}
void Player::Respawn() {
    Position(checkpoint).Velocity({0, 0});
    jumpBuffer_ = floorGrace_ = 0;
    ++ProgressOf().deaths;
    Engine::Get().Audio().Sound("hit").Play();
}
void Player::Tick(float dt, const Input& input) {
    jumpBuffer_ = std::max(0.0f, jumpBuffer_ - dt);
    if (input.KeyPressed(KEY_SPACE) || input.KeyPressed(KEY_W) || input.KeyPressed(KEY_UP))
        jumpBuffer_ = 0.12f;
    const Vector2 velocity = Velocity();
    if (std::fabs(velocity.x) > 1) sprite_->FlippedX(velocity.x < 0);
    const char* animation = "stoi";
    if (!IsOnFloor())
        animation = "skok";
    else if (std::fabs(velocity.x) > 1)
        animation = "bieg";
    sprite_->Play(animation);
}
void Player::FixedTick(float step, const Input& input) {
    floorGrace_ = IsOnFloor() ? 0.1f : std::max(0.0f, floorGrace_ - step);
    const int right = input.KeyDown(KEY_D) || input.KeyDown(KEY_RIGHT);
    const int left = input.KeyDown(KEY_A) || input.KeyDown(KEY_LEFT);
    Vector2 velocity = Velocity();
    velocity.x = static_cast<float>(right - left) * 270;
    velocity.y = std::min(velocity.y + Gravity().y * step, 900.0f);
    if (jumpBuffer_ > 0 && floorGrace_ > 0) {
        velocity.y = -620;
        jumpBuffer_ = floorGrace_ = 0;
        Engine::Get().Audio().Sound("jump").Play();
    }
    Velocity(velocity).MoveAndSlide();
    for (const auto& hit : SlideCollisions())
        if (auto* crate = dynamic_cast<node::physics::RigidBody*>(hit.other))
            crate->ApplyCentralForce({-hit.normal.x * 3600, 0});
    if (Position().y > 900) Respawn();
}
}
