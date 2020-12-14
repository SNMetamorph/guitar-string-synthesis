#pragma once
#include "synth_audio_stream.h"

class CUserInterface
{
public:
    static CUserInterface& GetInstance();
    void Initialize(CSynthAudioStream *stream);
    void KeyPressed(char keyChar);
    void Begin();

private: 
    CUserInterface() {};
    ~CUserInterface() {};
    void    ApplyConfig();
    void    UpdateState();
    void    UpdateWindowSize();
    int     GetNoteIndex(char keyChar);
    void    BeginSettingsWindow();
    void    BeginWaveformWindow();
    void    BeginKeyboardWindow();
    void    BeginAboutModal(const char *modalName);

    int                 m_iBufferSize = 0;
    int                 m_iWindowWidth = 0;
    int                 m_iWindowHeight = 0;
    size_t              m_iWaveformInterval = 256;
    float               m_flDecayFactor = 0;
    std::vector<float>  m_WaveformSamples;
    CSynthAudioStream*  m_pAudioStream = nullptr;
};
