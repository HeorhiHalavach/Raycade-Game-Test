/**
 * @file main.cpp
 * @brief Punkt wejścia: konfiguracja silnika i uruchomienie pętli gry.
 * @details Sceny rejestrują się same (makro SCENE w plikach src/scenes/) i są wczytywane
 * leniwie - przy pierwszym wejściu na nie.
 */

#include <raycade>

int main() {
    Engine& engine = Engine::Get()
                         .Title("Nowa gra")
                         .Window(1280, 720)
                         .MinSize(640, 360)
                         .Resolution(1280, 720)   // gra zawsze rysuje w 1280x720...
                         .ScaleMode(Scaling::Fit) // ...a obraz skaluje się do okna
                         .VSync()
                         .Background(RAYWHITE)
                         .StartScene("start")
                         .Init();

    if (!engine.IsReady()) return 1;

    // czcionka z polskimi znakami dla wszystkich napisów (DejaVu Sans, licencja w assets/)
    engine.ProvideContext<DefaultTypeface>(engine.Assets().Font("font.ttf"));

    return engine
        .OnUpdate([&engine](float) {
            if (engine.Input().KeyPressed(KEY_F11)) engine.Fullscreen(!engine.IsFullscreen());
        })
        .Run();
}
