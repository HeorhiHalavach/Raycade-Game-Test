#pragma once
#include "Demo.hpp"

namespace demo {
class Player : public node::physics::CharacterBody {
  public:
    Player();
    void Respawn();
    Vector2 checkpoint{100, 600};

  protected:
    void Tick(float dt, const Input& input) override;
    void FixedTick(float step, const Input& input) override;

  private:
    node::visual::Sprite* sprite_ = nullptr;
    float jumpBuffer_ = 0, floorGrace_ = 0;
};
}
