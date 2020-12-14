#include "guitar_string.h"

CGuitarString::CGuitarString(int stringFreq, int sampleRate)
{
    Initialize(stringFreq, sampleRate);
}

void CGuitarString::Initialize(int stringFreq, int sampleRate)
{
    m_iSampleCount = sampleRate / stringFreq;
    m_SynthSamples.SetCapacity(m_iSampleCount);
    m_SynthSamples.Assign(0.0f);
}

void CGuitarString::Pluck()
{
    float prevSample;
    for (int i = 0; i < m_SynthSamples.GetSize(); ++i) 
    {
        if (!m_SynthSamples.Dequeue(prevSample))
            continue;

        // random value from [0.0; 1.0]
        float newSample = rand() / (float)RAND_MAX;
        newSample -= 0.5f; // to [-0.5; 0.5]
        newSample *= 2.f;  // to [-1; 1]
        newSample *= m_flVolume;
        m_SynthSamples.Enqueue(newSample);
    }
}

void CGuitarString::RunSynthesis(std::vector<float> &mixerSamples)
{
    float firstSample, secondSample, newSample;
    for (int i = 0; i < mixerSamples.size(); ++i)
    {
        if (m_SynthSamples.Dequeue(firstSample))
        {
            m_SynthSamples.Peek(secondSample);
            newSample = 0.5f * (firstSample + secondSample);
            newSample *= m_flDecayFactor;
            m_SynthSamples.Enqueue(newSample);
            mixerSamples[i] += newSample;
        }
    }
}

