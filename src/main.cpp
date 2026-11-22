#include "Demo.hpp"

int main() {
    Engine& engine = Engine::Get()
                         .Title("Awaria stacji | Raycade")
                         .Window(1280, 720)
                         .MinSize(640, 360)
                         .Resolution(1280, 720)
                         .ScaleMode(Scaling::Fit)
                         .VSync()
                         .Background(demo::background)
                         .ExitKey(KEY_NULL)
                         .StartScene("start")
                         .Init();

    if (!engine.IsReady()) return 1;

    engine.ProvideContext<DefaultTypeface>(engine.Assets().Font("font.ttf"));
    engine.ProvideContext<demo::Progress>();
    Audio& audio = engine.Audio();
    audio.Volume(0.65f);
    audio.Bus("music").Volume(0.3f);
    audio.Bus("effects").Volume(0.65f);
    audio.Sound("click").Files({"sound/click.wav"}).Bus("effects").Volume(0.5f);
    audio.Sound("step")
        .Files({"sound/step1.wav", "sound/step2.wav"})
        .Bus("effects")
        .Volume(0.3f)
        .MinInterval(0.12f);
    audio.Sound("jump").Files({"sound/coin.wav"}).Bus("effects").Pitch(0.6f).Volume(0.3f);
    audio.Sound("cell").Files({"sound/coin.wav"}).Bus("effects");
    audio.Sound("hit").Files({"sound/hit.wav"}).Bus("effects").Volume(0.4f);
    audio.Sound("reactor").Files({"sound/fire.wav"}).Looped().Bus("effects").Volume(0.25f);
    audio.Music("sound/theme.wav").FadeInTime(1).Play();

    return engine
        .OnUpdate([&engine](float) {
            if (engine.Input().KeyPressed(KEY_F11)) engine.Fullscreen(!engine.IsFullscreen());
            if (engine.Input().KeyPressed(KEY_ESCAPE)) demo::Menu();
        })
        .Run();
}
