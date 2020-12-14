#pragma once
#include "ring_buffer.h"

class CGuitarString
{
public:
    CGuitarString() {};
    CGuitarString(int stringFreq, int sampleRate);

    void Initialize(int stringFreq, int sampleRate);
    void Pluck();
    void RunSynthesis(std::vector<float> &mixerSamples);
    inline void SetDecayFactor(float value) { m_flDecayFactor = value; }
    inline void SetVolume(float value) { m_flVolume = value; }

private:
    int         m_iSampleCount = 0;
    float       m_flVolume = 1.0f;
    float       m_flDecayFactor = 0.994f;
    CRingBuffer<float> m_SynthSamples;
};
