#pragma once
#include "Demo.hpp"
#include <array>

namespace demo {
class FusePanel : public node::Group {
  public:
    FusePanel();
    const std::string& Message() const { return message_; }

  protected:
    void Tick(float dt, const Input& input) override;

  private:
    void Drop(int index);
    std::array<node::shapes::Polygon*, 3> pieces_{};
    std::array<bool, 3> installed_{};
    int selected_ = -1;
    std::string message_ = "Przeciągnij · Q/E obrót";
};
}
