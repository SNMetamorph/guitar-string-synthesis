#include "user_interface.h"
#include "imgui.h"
#include "imgui-SFML.h"
#include <algorithm>

CUserInterface& CUserInterface::GetInstance()
{
    static CUserInterface instance;
    return instance;
}

void CUserInterface::Initialize(CSynthAudioStream *stream)
{
    m_pAudioStream = stream;
    m_iBufferSize = stream->GetBufferSize();
    m_flDecayFactor = stream->GetDecayFactor();
    m_WaveformSamples.resize(m_iWaveformInterval);
    ApplyConfig();
}

void CUserInterface::KeyPressed(char keyChar)
{
    int noteIndex = GetNoteIndex(keyChar);
    if (noteIndex >= 0 && noteIndex < m_pAudioStream->GetNoteCount()) {
        m_pAudioStream->PluckNote(noteIndex);
    }
}

void CUserInterface::Begin()
{
    UpdateState();
    BeginSettingsWindow();
    BeginWaveformWindow();
    BeginKeyboardWindow();
    //ImGui::ShowDemoWindow();
}

void CUserInterface::ApplyConfig()
{
    ImGuiStyle &style = ImGui::GetStyle();
    ImVec4 *colors = style.Colors;
    ImGuiIO &io = ImGui::GetIO();

    // disable saving windows layout to file
    io.IniFilename = NULL;
    io.LogFilename = NULL;

    io.Fonts->Clear();
    io.Fonts->AddFontFromFileTTF("fonts/Roboto-Medium.ttf", 15.0f, 0, io.Fonts->GetGlyphRangesCyrillic());
    io.Fonts->AddFontDefault();
    ImGui::SFML::UpdateFontTexture();

    style.AntiAliasedFill = true;
    style.AntiAliasedLines = true;
    style.AntiAliasedLinesUseTex = false;

    style.TabRounding = 0;
    style.GrabRounding = 0;
    style.ChildRounding = 0;
    style.FrameRounding = 0;
    style.PopupRounding = 0;
    style.WindowRounding = 0;
    style.ScrollbarRounding = 0;

    colors[ImGuiCol_FrameBg] = ImVec4(0.00f, 0.54f, 0.06f, 0.54f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.98f, 0.35f, 0.40f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.98f, 0.35f, 0.67f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.16f, 0.48f, 0.20f, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.26f, 0.98f, 0.35f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.24f, 0.88f, 0.32f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.26f, 0.98f, 0.35f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.26f, 0.98f, 0.35f, 0.40f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.98f, 0.35f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.98f, 0.17f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.26f, 0.98f, 0.35f, 0.31f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.98f, 0.35f, 0.80f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.98f, 0.35f, 1.00f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.75f, 0.18f, 0.78f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.75f, 0.18f, 1.00f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.98f, 0.35f, 0.25f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.98f, 0.35f, 0.67f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.98f, 0.35f, 0.95f);
    colors[ImGuiCol_Tab] = ImVec4(0.18f, 0.58f, 0.23f, 0.86f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.26f, 0.98f, 0.35f, 0.80f);
    colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.68f, 0.26f, 1.00f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.07f, 0.15f, 0.08f, 0.97f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.42f, 0.17f, 1.00f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.98f, 0.35f, 0.35f);
    colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.98f, 0.35f, 1.00f);
}

void CUserInterface::UpdateState()
{
    UpdateWindowSize();
    m_pAudioStream->CopySamples(m_WaveformSamples);
}

void CUserInterface::UpdateWindowSize()
{
    ImGuiIO &io = ImGui::GetIO();
    m_iWindowWidth = static_cast<int>(io.DisplaySize.x);
    m_iWindowHeight = static_cast<int>(io.DisplaySize.y);
}

int CUserInterface::GetNoteIndex(char keyChar)
{
    static const std::string layout = "q2w3er5t6y7ui9o0p[";
    for (int i = 0; i < layout.length(); ++i)
    {
        if (keyChar == layout[i])
            return i;
    }
    return -1;
}

void CUserInterface::BeginSettingsWindow()
{
    const char *windowName = "Settings";
    const ImGuiWindowFlags windowFlags = (
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoTitleBar
    );
    
    ImGui::SetNextWindowPos(ImVec2());
    ImGui::SetNextWindowSize(ImVec2(0.f, 0.f));
    ImGui::Begin(windowName, nullptr, windowFlags);
    ImGui::SliderInt("Buffer size", &m_iBufferSize, 300, 1500);
    if (ImGui::IsItemDeactivated())
    {
        m_pAudioStream->stop();
        m_pAudioStream->SetBufferSize(m_iBufferSize);
        m_pAudioStream->play();
        if (m_iWaveformInterval > m_iBufferSize)
            m_iWaveformInterval = m_iBufferSize;
    }

    if (ImGui::SliderFloat("Decay factor", &m_flDecayFactor, 0.75f, 1.0f, "%.4f", ImGuiSliderFlags_Logarithmic))
        m_pAudioStream->SetDecayFactor(m_flDecayFactor);

    ImGui::SliderInt("Waveform interval", (int*)&m_iWaveformInterval, 32, m_iBufferSize, "%d samples");
    if (ImGui::IsItemDeactivated())
        m_WaveformSamples.resize(m_iWaveformInterval);

    const char *aboutButtonLabel = "About";
    if (ImGui::Button(aboutButtonLabel))
        ImGui::OpenPopup(aboutButtonLabel);
    BeginAboutModal(aboutButtonLabel);

    ImGui::End();
}

void CUserInterface::BeginWaveformWindow()
{
    int sampleCount = std::min(m_iWaveformInterval, m_WaveformSamples.size());
    float plotWidth = m_iWindowWidth * 1.0f;
    float plotHeight = m_iWindowHeight * 0.6f;
    const char *windowName = "Waveform";
    const ImGuiWindowFlags windowFlags = (
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoBackground |
        ImGuiWindowFlags_NoBringToFrontOnFocus
    );
    
    ImGui::SetNextWindowPos(ImVec2(0.f, m_iWindowHeight * .5f - plotHeight / 2));
    ImGui::SetNextWindowSize(ImVec2(plotWidth, 0.f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::Begin(windowName, nullptr, windowFlags);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.f, 0.f));
        ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.f, 0.f, 0.f, 0.f));
        ImGui::PlotLines("", m_WaveformSamples.data(), sampleCount, 0, nullptr, -1.0, 1.0, ImVec2(plotWidth, plotHeight));
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();
    ImGui::End();
    ImGui::PopStyleVar(2);
}

void CUserInterface::BeginKeyboardWindow()
{
    size_t noteCount = m_pAudioStream->GetNoteCount();
    const char *windowName = "Keyboard";
    const float keyboardWidth = 600.f;
    const float keyboardHeight = 130.f;
    static const std::array<const char*, 24> noteNames = {
        "C3", "C#3", "D3",
        "D#3", "E3", "F3",
        "F#3", "G3", "G#3",
        "A3", "A#3", "B3", 
        "C4", "C#4", "D4", 
        "D#4", "E4", "F4", 
        "F#4", "G4", "G#4",
        "A4", "B4", "C5"
    };
    
    ImGui::SetNextWindowPos(
        ImVec2(
            m_iWindowWidth / 2 - keyboardWidth / 2, 
            m_iWindowHeight - keyboardHeight
        ), ImGuiCond_Once
    );
    ImGui::SetNextWindowSize(ImVec2(keyboardWidth, keyboardHeight), ImGuiCond_Once);
    ImGui::Begin(windowName, nullptr);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(1.f, 0));
    float availWidth = ImGui::GetContentRegionAvailWidth();
    float itemSpacingX = ImGui::GetStyle().ItemSpacing.x;
    float buttonWidth = (availWidth - itemSpacingX * (noteCount - 1)) / noteCount;
    for (int i = 0; i < noteCount; ++i)
    {
        ImGui::PushID(i);
        if (ImGui::Button(noteNames[i], ImVec2(buttonWidth, -1.f))) {
            m_pAudioStream->PluckNote(i);
        }
        ImGui::PopID();
        ImGui::SameLine();
    }
    ImGui::PopStyleVar();
    ImGui::End();
}

void CUserInterface::BeginAboutModal(const char *modalName)
{
    if (ImGui::BeginPopupModal(modalName, NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text(
            "Guitar string sound synthesis\n"
            "Based on a Karplus Strong algorithm\n"
            "Created by: SNMetamorph\n"
            "Compiled in: " __DATE__ " " __TIME__
            "\n\n"
        );
        ImGui::Separator();
        if (ImGui::Button("OK", ImVec2(-1.f, 0))) { 
            ImGui::CloseCurrentPopup(); 
        }
        ImGui::SetItemDefaultFocus();
        ImGui::EndPopup();
    }
}
