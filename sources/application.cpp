#include "application.h" 
#include "user_interface.h"
#include "imgui-SFML.h"
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

CApplication& CApplication::GetInstance()
{
    static CApplication instance;
    return instance;
}

int CApplication::Run(int argc, char *argv[])
{
    const int windowWidth = 1024;
    const int windowHeight = 768;
    const int framesPerSecond = 60;
    const char *windowTitle = "Guitar String Synthesis";
    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), windowTitle);
    CUserInterface &ui = CUserInterface::GetInstance();

    window.setFramerateLimit(framesPerSecond);
    StartAudioStream(audioStream, 44100, 1);
    ImGui::SFML::Init(window, false);
    ui.Initialize(&audioStream);
    RunMainLoop(window);
    ImGui::SFML::Shutdown();
    StopAudioStream(audioStream);
    return 0;
}

void CApplication::RunMainLoop(sf::RenderWindow& window)
{
    sf::Event event;
    sf::Clock deltaClock;
    CUserInterface &ui = CUserInterface::GetInstance();
    while (window.isOpen())
    {
        while (window.pollEvent(event))
        {
            ImGui::SFML::ProcessEvent(event);
            if (event.type == sf::Event::Closed)
                window.close();
            else if (event.type == sf::Event::TextEntered)
            {
                if (event.text.unicode < 128)
                    ui.KeyPressed(static_cast<char>(event.text.unicode));
            }
        }

        ImGui::SFML::Update(window, deltaClock.restart());
        CUserInterface::GetInstance().Begin();
        window.clear();
        ImGui::SFML::Render(window);
        window.display();
    }
}

void CApplication::StartAudioStream(CSynthAudioStream &stream, int sampleRate, int channelCount)
{
    stream.Initialize(sampleRate, channelCount);
    stream.play();
}

void CApplication::StopAudioStream(CSynthAudioStream &stream)
{
    stream.stop();
    while (stream.getStatus() != sf::SoundStream::Stopped)
        sf::sleep(sf::seconds(0.1f));
}

