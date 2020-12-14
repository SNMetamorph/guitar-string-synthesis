#include "synth_audio_stream.h"
#include <algorithm>

void CSynthAudioStream::Initialize(int sampleRate, int channelCount)
{
    initialize(channelCount, sampleRate);
    InitializeNotes(sampleRate);
    SetBufferSize(m_iBufferSize);
    SetDecayFactor(m_flDecayFactor);
}

void CSynthAudioStream::PluckNote(size_t index)
{
    m_StreamMutex.lock();
    m_Notes[index].Pluck();
    m_StreamMutex.unlock();
}

void CSynthAudioStream::SetBufferSize(int bufferSize)
{
    m_StreamMutex.lock();
    m_OutputBuffer.resize(bufferSize);
    m_MixerBuffer.resize(bufferSize);
    m_iBufferSize = bufferSize;
    m_StreamMutex.unlock();
}

void CSynthAudioStream::SetDecayFactor(float decayFactor)
{
    m_StreamMutex.lock();
    for (size_t i = 0; i < m_Notes.size(); ++i)
        m_Notes[i].SetDecayFactor(decayFactor);
    m_StreamMutex.unlock();
}

void CSynthAudioStream::SetVolume(float volume)
{
    m_StreamMutex.lock();
    for (size_t i = 0; i < m_Notes.size(); ++i)
        m_Notes[i].SetVolume(volume);
    m_StreamMutex.unlock();
}

void CSynthAudioStream::CopySamples(std::vector<float> &destSamples)
{
    m_StreamMutex.lock();
    size_t bytesCount = std::min(destSamples.size(), (size_t)m_iBufferSize) * sizeof(destSamples[0]);
    memcpy(destSamples.data(), m_MixerBuffer.data(), bytesCount);
    m_StreamMutex.unlock();
}

bool CSynthAudioStream::onGetData(Chunk& audioData)
{
    m_StreamMutex.lock();
    ClearMixerBuffer();
    RunSynthesis();
    WriteOutputSamples();
    audioData.samples = &m_OutputBuffer[0];
    audioData.sampleCount = m_OutputBuffer.size();
    m_StreamMutex.unlock();
    return true;
}

void CSynthAudioStream::onSeek(sf::Time timeOffset)
{
}

void CSynthAudioStream::InitializeNotes(int sampleRate)
{
    const float baseFreq = 130.81f; // note C3
    for (int i = 0; i < m_Notes.size(); ++i) {
        m_Notes[i].Initialize(GetNoteFrequency(i, baseFreq), sampleRate);
    }
}

void CSynthAudioStream::ClearMixerBuffer()
{
    for (size_t i = 0; i < m_MixerBuffer.size(); ++i)
        m_MixerBuffer[i] = 0.0f;
}

void CSynthAudioStream::RunSynthesis()
{
    for (size_t i = 0; i < m_Notes.size(); ++i)
        m_Notes[i].RunSynthesis(m_MixerBuffer);
}

void CSynthAudioStream::WriteOutputSamples()
{
    float maxAmpl = GetMixerMaxAmplitude();
    for (int i = 0; i < m_OutputBuffer.size(); ++i)
    {
        float currSample = m_MixerBuffer[i];
        if (maxAmpl > 1.0f)
            currSample /= maxAmpl; // normalize in-a-place
        m_OutputBuffer[i] = static_cast<sf::Int16>(currSample * 32767);
    }
}

float CSynthAudioStream::GetMixerMaxAmplitude()
{
    float maxAmplitude = 0.0f;
    for (size_t i = 0; i < m_MixerBuffer.size(); ++i)
    {
        float sampleAmplitude = std::fabsf(m_MixerBuffer[i]);
        if (sampleAmplitude > maxAmplitude)
            maxAmplitude = sampleAmplitude;
    }
    return maxAmplitude;
}

int CSynthAudioStream::GetNoteFrequency(int index, float baseFreq)
{
    return std::lroundf(baseFreq * std::powf(2.f, index / 12.f));
}
