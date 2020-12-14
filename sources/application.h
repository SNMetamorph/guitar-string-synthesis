#pragma once
#include "synth_audio_stream.h"
#include <SFML/Graphics/RenderWindow.hpp>

class CApplication
{
public:
    static CApplication& GetInstance();
    int Run(int argc, char* argv[]);

private: 
    CApplication() {};
    ~CApplication() {};

    void RunMainLoop(sf::RenderWindow& window);
    void StartAudioStream(CSynthAudioStream &stream, int sampleRate, int channelCount);
    void StopAudioStream(CSynthAudioStream &stream);

    CSynthAudioStream audioStream;
};
