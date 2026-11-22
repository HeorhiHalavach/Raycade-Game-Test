#include "FusePanel.hpp"
#include <algorithm>
#include <cmath>
#include <format>

namespace {
constexpr std::array<Vector2, 3> homes{Vector2{300, 310}, {300, 430}, {300, 550}};
constexpr std::array<Vector2, 3> slots{Vector2{920, 430}, {920, 550}, {920, 310}};
constexpr std::array<float, 3> angles{PI / 2, PI, -PI / 2};
constexpr std::array<Color, 3> colors{demo::cyan, demo::amber, Color{177, 140, 246, 255}};
}

namespace demo {
FusePanel::FusePanel() {
    const std::array<std::vector<Point2D>, 3> shapes{
        std::vector<Point2D>{{-36, -30}, {40, 0}, {-36, 30}},
        std::vector<Point2D>{{-35, -35}, {-10, -35}, {-10, 10}, {35, 10}, {35, 35}, {-35, 35}},
        std::vector<Point2D>{{-38, -16}, {6, -16}, {6, -35}, {40, 0}, {6, 35}, {6, 16}, {-38, 16}}
    };
    for (int i = 0; i < 3; ++i) {
        Spawn<node::shapes::Polygon>()
            .Vertices(shapes[i])
            .Position(slots[i])
            .Rotation(angles[i])
            .Paint(border)
            .Filled()
            .Raster(Rasterizer::Custom);
        Spawn<node::shapes::Polygon>()
            .Vertices(shapes[i])
            .Position(slots[i])
            .Rotation(angles[i])
            .Paint(colors[i])
            .Raster(Rasterizer::Custom);
        auto& piece = Spawn<node::shapes::Polygon>()
                          .Vertices(shapes[i])
                          .Position(homes[i])
                          .Paint(colors[i])
                          .Filled()
                          .Raster(Rasterizer::Custom)
                          .Layer(1);
        pieces_[i] = &piece;
        piece.Spawn<node::shapes::Circle>().Position({-22, 0}).Radius(4).Paint(background).Filled();
        piece.OnDragStart([this, i](auto& self) {
            if (!installed_[i]) {
                selected_ = i;
                self.Layer(2);
            }
        });
        piece.OnDrag([this, i](auto& self, Vector2 delta) {
            if (!installed_[i]) self.Translate(delta);
        });
        piece.OnDragEnd([this, i](auto&) { Drop(i); });
    }
}

void FusePanel::Tick(float, const Input& input) {
    if (!IsVisible()) return;
    const int turn =
        static_cast<int>(input.KeyPressed(KEY_E)) - static_cast<int>(input.KeyPressed(KEY_Q));
    if (turn && selected_ >= 0 && !installed_[selected_])
        pieces_[selected_]->Rotate(static_cast<float>(turn) * PI / 2);
}

void FusePanel::Drop(int index) {
    if (installed_[index]) return;
    auto& piece = *pieces_[index];
    const Vector2 position = piece.Position(), slot = slots[index];
    piece.Layer(1);
    const Vector2 home = homes[index];
    if (std::hypot(position.x - home.x, position.y - home.y) < 1) return;
    const bool near = std::hypot(position.x - slot.x, position.y - slot.y) < 32;
    const bool aligned =
        std::fabs(std::remainder(piece.Rotation() - angles[index], 2 * PI)) < 0.05f;
    if (!near || !aligned) {
        piece.Position(homes[index]);
        message_ = near ? "Q/E — obróć część" : "Inne gniazdo";
        return;
    }
    piece.Position(slot).Rotation(angles[index]);
    installed_[index] = true;
    selected_ = -1;
    Engine::Get().Audio().Sound("click").Play();
    const int count = static_cast<int>(std::count(installed_.begin(), installed_.end(), true));
    message_ = std::format("Bezpieczniki: {}/3", count);
    if (count == 3) {
        ProgressOf().repaired = true;
        message_ = "Gotowe";
    }
}
}
