#pragma once
#include "guitar_string.h"
#include <SFML/Audio.hpp>
#include <vector>
#include <mutex>
#include <array>

class CSynthAudioStream : public sf::SoundStream
{
public: 
    void Initialize(int sampleRate, int channelCount);
    void SetBufferSize(int bufferSize);
    void SetDecayFactor(float decayFactor);
    void SetVolume(float volume);
    void CopySamples(std::vector<float> &destSamples);
    void PluckNote(size_t index);

    inline int GetBufferSize() const { return m_iBufferSize; }
    inline float GetDecayFactor() const { return m_flDecayFactor; }
    inline size_t GetNoteCount() const { return m_Notes.size(); }

private:
    virtual bool onGetData(Chunk& audioData);
    virtual void onSeek(sf::Time timeOffset);

    void InitializeNotes(int sampleRate);
    void ClearMixerBuffer();
    void RunSynthesis();
    void WriteOutputSamples();
    float GetMixerMaxAmplitude();
    int GetNoteFrequency(int index, float baseFreq);

    std::array<CGuitarString, 18> m_Notes;
    std::vector<float>      m_MixerBuffer;
    std::vector<sf::Int16>  m_OutputBuffer;
    std::mutex              m_StreamMutex;
    int                     m_iBufferSize = 512;
    float                   m_flDecayFactor = 0.993f;
};
