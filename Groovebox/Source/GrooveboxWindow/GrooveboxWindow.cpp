#include "GrooveboxWindow.h"

#include "Disk.h"
#include "Input/InputSystem.h"
#include "Sequence.h"
#include "SequencerStream.h"
#include "Util.h"
#include "UI/DialWidget.h"
#include "UI/LabelWidget.h"
#include "UI/LayoutWidget.h"
#include "UI/ModalDialogWidget.h"
#include "UI/ModulationSourceButtonWidget.h"
#include "UI/SequencerToggleWidget.h"
#include "UI/TabbedPanelWidget.h"
#include "UI/Widget.h"
#include "Voice/SynthParamReflection.h"

#include <sstream>

using namespace nlohmann;
using namespace sf;
using namespace std;

namespace
{
    template <typename ReflectionCategoryType>
    void BuildDials(GrooveboxWindow& window, Widget* parentWidget, const string& categoryPrefix, const ReflectionCategoryType& reflectionCategory)
    {
        for (const SynthParamReflection::IValue* iValue : reflectionCategory.GetValues())
        {
            const ReflectionCategoryType::ValueType* value = static_cast<const ReflectionCategoryType::ValueType*>(iValue);
            stringstream ss;
            ss << categoryPrefix << value->GetShortName();
            window.BuildDial(
                parentWidget,
                ss.str(),
                [reflectionCategory, value](SynthVoiceParams& params) -> SynthVoiceParam&
                {
                    return value->GetSynthVoiceParam(reflectionCategory.GetParamsCategory(params));
                },
                [reflectionCategory, value](SynthVoiceOptionalParams& params) -> SynthVoiceOptionalParam&
                { 
                    return value->GetSynthVoiceOptionalParam(reflectionCategory.GetOptionalParamsCategory(params));
                },
                [reflectionCategory, value](SynthVoiceModulationShortParams& params) -> short&
                { 
                    return value->GetSynthVoiceModulationShortParam(reflectionCategory.GetModulationShortParamsCategory(params));
                }
            );
        }
    }

    void BuildGlobalDials(GrooveboxWindow& window, Widget* parentWidget)
    {
        DialWidget& dial = window.BuildSimpleDial(
            parentWidget,
            "Tempo",
            [&window]() -> SynthVoiceParam&
            {
                return window.GetSequencerStream().GetTempoParam();
            });
    }
}

GrooveboxWindow::GrooveboxWindow(RenderWindow& sfmlWindow, SequencerStream& sequencerStream, array<Sequence, Constants::NumTracks>& sequences)
    : m_sfmlWindow(sfmlWindow),
    m_sequencerStream(sequencerStream),
    m_sequences(sequences),
    m_rootWidget(new Widget(nullptr)),
    m_noteStartedHandler(m_sequencerStream.OnNoteStarted(), [this](NoteStartedEvent event) { HandleNoteStarted(event); }),
    m_noteEndedHandler(m_sequencerStream.OnNoteEnded(), [this](NoteEndedEvent event) { HandleNoteEnded(event); }),
    m_stepChangedHandler(m_sequencerStream.OnStepChanged(), [this](StepChangedEvent event) { HandleStepChanged(event); })
{
    m_rootWidget->SetBoundsDelta(0.0f, 0.0f, (float)sfmlWindow.getSize().x, (float)sfmlWindow.getSize().y);

    m_sequencerTogglesLayout = new LayoutWidget(
        m_rootWidget,
        LayoutWidget::Orientation::Horizontal,
        LayoutWidget::HorizontalAlignment::Center,
        LayoutWidget::VerticalAlignment::Middle,
        false,
        false,
        Vector2f(100.0f, 100.0f),
        FloatRect(10.0f, 10.0f, -20.0f, -20.0f));

    m_sequencerTogglesLayout->SetAnchors(0.0f, 1.0f, 1.0f, 1.0f);
    m_sequencerTogglesLayout->SetBoundsDelta(0.0f, -SequencerTogglesLayoutHeight, 0.0f, SequencerTogglesLayoutHeight);

    for (int i = 0; i < GetCurrentSequence().GetLength(); ++i)
    {
        SequencerToggleWidget* toggle = new SequencerToggleWidget(m_sequencerTogglesLayout, i, *this);
        toggle->SetStepOnPipIsOn(GetCurrentSequence().GetStep(i).GetOn());
        m_sequencerToggles.push_back(toggle);
    }

    const std::vector<ModulationSource*>& trackModulationSources = m_sequencerStream.GetVoice(m_currentTrack).GetModulationSources();

    for (size_t modulationSourceIndex = 0; modulationSourceIndex < trackModulationSources.size(); ++modulationSourceIndex)
    {
        std::array<ModulationSource*, Constants::NumTracks> modulationSources;

        for (size_t trackIndex = 0; trackIndex < Constants::NumTracks; ++trackIndex)
        {
            modulationSources[trackIndex] = m_sequencerStream.GetVoice(trackIndex).GetModulationSources()[modulationSourceIndex];
        }

        BuildModulationSourceButton(modulationSources);
    }

    m_tabbedPanel = new TabbedPanelWidget(m_rootWidget);
    m_tabbedPanel->SetPointAnchor(0.0f, 0.0f);
    m_tabbedPanel->SetBoundsDelta(50.0f, 50.0f, 500.f, 500.0f);

    Widget* tab = m_tabbedPanel->AddPanel(L"Global");
    BuildGlobalDials(*this, tab);

    NextDialTab();
    tab = m_tabbedPanel->AddPanel(L"Synth");
    const SynthParamReflection::Reflection& reflection = SynthParamReflection::Reflection::Instance();
    BuildDials(*this, tab, "", reflection.GetSynth());

    NextDialTab();
    tab = m_tabbedPanel->AddPanel(L"Oscs");
    BuildDials(*this, tab, "Osc1", reflection.GetOscillator1());
    NextDialRow(); 
    BuildDials(*this, tab, "Osc2", reflection.GetOscillator2());

    NextDialTab();
    tab = m_tabbedPanel->AddPanel(L"Envelopes");
    BuildDials(*this, tab, "Env1", reflection.GetEnvelope1());
    NextDialRow();
    BuildDials(*this, tab, "Env2", reflection.GetEnvelope2());

    NextDialTab();
    tab = m_tabbedPanel->AddPanel(L"Lfo");
    BuildDials(*this, tab, "Lfo", reflection.GetLfo());

    NextDialTab();
    tab = m_tabbedPanel->AddPanel(L"Filter");
    BuildDials(*this, tab, "", reflection.GetFilter());

    NextDialTab();
    tab = m_tabbedPanel->AddPanel(L"Delay");
    BuildDials(*this, tab, "", reflection.GetDelay());

    m_tabbedPanel->AddButton(
        L"Save Sound",
        [this]()
        {
            this->m_modalDialogWidget->ShowModalDialog(L"Save Current Sound", true, [this](const std::wstring& soundName)
                {
                    this->SaveSound(soundName);
                });
            return true;
        });

    tab = m_tabbedPanel->AddPanel(L"Load Sound");

    m_modalDialogWidget = new ModalDialogWidget(m_rootWidget);
    m_modalDialogWidget->SetIsActive(false);

    ButtonWidget* loadSoundButton = new ButtonWidget(
        tab,
        [this](int mouseButtonIndex)
        {
            this->m_modalDialogWidget->ShowModalDialog(L"Load Sound", true, [this](const std::wstring& soundName)
                {
                    this->LoadSound(soundName);
                });
            return true;
        },
        L"Load Sound");

    loadSoundButton->SetPointAnchor(0.5f, 0.5f);
    loadSoundButton->SetBoundsDelta(-100.0f, 100.0f, 200.0f, 100.0f);
}

GrooveboxWindow::~GrooveboxWindow() { }

void GrooveboxWindow::Tick(double deltaSeconds)
{
    ProcessInput();
    Update(deltaSeconds);
    Draw();
}

void GrooveboxWindow::ProcessInput()
{
    Event event;

    InputSystem& inputSystem = InputSystem::Instance();

    while (m_sfmlWindow.pollEvent(event))
    {
        if (inputSystem.HandleInputEvent(event, m_sfmlWindow, *m_rootWidget))
        {
            continue;
        }

        switch (event.type)
        {
        case Event::Closed:
            m_sfmlWindow.close();
            break;
        
        case Event::Resized:
        {
            FloatRect visibleArea(0.0f, 0.0f, (float)event.size.width, (float)event.size.height);
            m_sfmlWindow.setView(View(visibleArea));
            m_rootWidget->SetBoundsDelta(visibleArea);
        }
        case Event::KeyPressed:
        {
            if (event.key.code >= Keyboard::Num1 && event.key.code <= (Keyboard::Num1 + Constants::NumTracks - 1))
            {
                SetCurrentTrack(event.key.code - Keyboard::Num1);
            }
        }
        break;
        }
    }
}

void GrooveboxWindow::Update(double deltaSeconds)
{
    m_rootWidget->CheckBoundsHaveChanged();
}

void GrooveboxWindow::Draw()
{
    m_sfmlWindow.clear(Color::Black);

    m_rootWidget->Draw(m_sfmlWindow);

    m_sfmlWindow.display();
}

bool GrooveboxWindow::SequencerToggleClicked(int stepIndex, int mouseButton)
{
    cout << "Toggle " << mouseButton << " clicked: step " << stepIndex << endl;

    switch (mouseButton)
    {
    case 0:
    {
        // Left click.
        // Toggle whether the step is on
        SequenceStep& step = GetCurrentSequence().GetStep(stepIndex);
        step.SetOn(!step.GetOn());
        m_sequencerToggles[stepIndex]->SetStepOnPipIsOn(step.GetOn());

        // Select only this step.
        ClearSelectedButtons();
        SetSequencerStepIsSelected(stepIndex, true);
        UpdateDialOverriddenValues();
        return true;
    }
    case 1:
        if (Util::IsShiftPressed())
        {
            // Shift + right click.
            // Leave other steps selected, and toggle the selected state of this one.
            ToggleSequencerStepIsSelected(stepIndex);
            UpdateDialOverriddenValues();
        }
        else
        {
            // Right click.
            // Select only this step.
            ClearSelectedButtons();
            SetSequencerStepIsSelected(stepIndex, true);
            UpdateDialOverriddenValues();
        }
        return true;
    case 2:
    {
        SequenceStep& step = GetCurrentSequence().GetStep(stepIndex);

        for (auto dial : m_dials)
        {
            if (!dial->IsSimpleDial())
            {
                SynthVoiceOptionalParam& param = (SynthVoiceOptionalParam&)dial->GetParam(step);
                param.ClearValue();
                dial->UpdateOverriddenValue();
            }
        }

        m_sequencerToggles[stepIndex]->GetSelectableButton().SetHasParamLock(false);
    }
    break;
    }

    return false;
}

void GrooveboxWindow::ClearSelectedButtons()
{
    for (auto toggle : m_sequencerToggles)
    {
        toggle->GetSelectableButton().SetIsSelected(false);
    }

    for (auto modSource : m_modulationSourceButtons)
    {
        modSource->GetSelectableButton().SetIsSelected(false);
    }
}

bool GrooveboxWindow::GetSequencerStepIsSelected(int stepIndex) const { return m_sequencerToggles[stepIndex]->GetSelectableButton().GetIsSelected(); }

void GrooveboxWindow::SetSequencerStepIsSelected(int stepIndex, bool isSelected) { m_sequencerToggles[stepIndex]->GetSelectableButton().SetIsSelected(isSelected); }

void GrooveboxWindow::ToggleSequencerStepIsSelected(int stepIndex) { SetSequencerStepIsSelected(stepIndex, !GetSequencerStepIsSelected(stepIndex)); }

bool GrooveboxWindow::ModulationSourceButtonClicked(ModulationSourceButtonWidget& button, int mouseButton)
{
    ModulationSource& modulationSource = button.GetModulationSource(m_currentTrack);
    cout << "Modulation source " << modulationSource.GetName() << " clicked" << endl;

    SelectableButtonWidget& selectableButton = button.GetSelectableButton();

    if (mouseButton == 0 || mouseButton == 1)
    {
        if (Util::IsShiftPressed())
        {
            selectableButton.ToggleIsSelected();
        }
        else
        {
            ClearSelectedButtons();
            selectableButton.SetIsSelected(true);
        }

        UpdateDialOverriddenValues();
    }
    else if (mouseButton == 2)
    {
        ModulationSource& modulationSource = button.GetModulationSource(m_currentTrack);

        for (auto dial : m_dials)
        {
            short& param = dial->GetParam(modulationSource.GetModulations());
            param = 0;
            dial->UpdateOverriddenValue();
        }

        selectableButton.SetHasParamLock(false);

        return true;
    }

    return false;
}

void GrooveboxWindow::UpdateDialOverriddenValues()
{
    for (auto dial : m_dials)
    {
        dial->UpdateOverriddenValue();
    }
}

void GrooveboxWindow::DialValueChanged(DialWidget& dial, int deltaValue, int mouseButton)
{
    switch (mouseButton)
    {
    case 0:
    {
        SynthVoiceParam& param = dial.IsSimpleDial()
            ? dial.GetSimpleParam()
            : dial.GetParam(m_sequencerStream.GetVoice(m_currentTrack).GetParams());

        param.SetValue(param.Value() + deltaValue);
        dial.SetValue(param.Value());
    }
    break;
    case 1:
        if (!dial.IsSimpleDial())
        {
            for (auto toggle : m_sequencerToggles)
            {
                if (toggle->GetSelectableButton().GetIsSelected())
                {
                    SequenceStep& step = GetCurrentSequence().GetStep(toggle->GetStepIndex());
                    SynthVoiceOptionalParam& param = dial.GetParam(step);

                    if (param.HasValue())
                    {
                        param.SetValue(param.Value() + deltaValue);
                    }
                    else
                    {
                        param.SetValue(dial.GetValue() + deltaValue);
                    }

                    toggle->GetSelectableButton().SetHasParamLock(true);
                }
            }

            for (auto modulationSource : m_modulationSourceButtons)
            {
                if (modulationSource->GetSelectableButton().GetIsSelected())
                {
                    SynthVoiceModulationShortParams& modulations = modulationSource->GetModulationSource(m_currentTrack).GetModulations();
                    short& param = dial.GetParam(modulations);
                    param += deltaValue;
                    modulations.Clamp();
                    modulationSource->GetSelectableButton().SetHasParamLock(true);
                }
            }

            dial.UpdateOverriddenValue();
        }
    }
}

bool GrooveboxWindow::ValueIsOverridden(SynthOptionalParamGetter optionalParamGetter, SynthModulationShortParamGetter shortParamGetter, string& overriddenValueString)
{
    int numSelectedToggles = 0;
    int numOverriddenToggles = 0;
    bool differentValue = false;
    short overriddenValue = 0;

    for (auto toggle : m_sequencerToggles)
    {
        if (toggle->GetSelectableButton().GetIsSelected())
        {
            SequenceStep& step = GetCurrentSequence().GetStep(toggle->GetStepIndex());
            const SynthVoiceOptionalParam& param = optionalParamGetter(step.GetParams());
            if (param.HasValue())
            {
                if (numSelectedToggles == 0)
                {
                    overriddenValue = param.Value();
                }
                else if (param.Value() != overriddenValue)
                {
                    differentValue = true;
                }

                ++numOverriddenToggles;
            }

            ++numSelectedToggles;
        }
    }

    int numSelectedModSources = 0;
    int numOverriddenModSources = 0;
    short modulatedValue = 0;

    for (auto modulationSource : m_modulationSourceButtons)
    {
        if (modulationSource->GetSelectableButton().GetIsSelected())
        {
            SynthVoiceModulationShortParams& modulations = modulationSource->GetModulationSource(m_currentTrack).GetModulations();
            short& param = shortParamGetter(modulations);

            if (param != 0)
            {
                if (numSelectedModSources == 0)
                {
                    overriddenValue = param;
                }
                else if (param != overriddenValue)
                {
                    differentValue = true;
                }

                ++numOverriddenModSources;
            }

            ++numSelectedModSources;
        }
    }

    if (numOverriddenToggles > 0)
    {
        if (differentValue || numOverriddenToggles != numSelectedToggles || numOverriddenModSources > 0)
        {
            overriddenValueString = "...";
        }
        else if (numOverriddenToggles != 0)
        {
            stringstream ss;
            ss << overriddenValue;
            overriddenValueString = ss.str();
        }
        else
        {
            overriddenValueString = "";
        }
    }
    else if (numOverriddenModSources > 0)
    {
        if (differentValue || numOverriddenModSources != numSelectedModSources)
        {
            overriddenValueString = "...";
        }
        else if (numOverriddenModSources != 0)
        {
            stringstream ss;
            ss << (overriddenValue > 0 ? "+" : "") << overriddenValue;
            overriddenValueString = ss.str();
        }
        else
        {
            overriddenValueString = "";
        }
    }
    else
    {
        overriddenValueString = "";
    }

    return numOverriddenToggles > 0 || numOverriddenModSources > 0;
}

bool GrooveboxWindow::HandleDialClicked(DialWidget& dial, int mouseButton)
{
    if (mouseButton == 2 && !dial.IsSimpleDial())
    {
        for (auto toggle : m_sequencerToggles)
        {
            if (toggle->GetSelectableButton().GetIsSelected())
            {
                SequenceStep& step = GetCurrentSequence().GetStep(toggle->GetStepIndex());
                SynthVoiceOptionalParam& param = (SynthVoiceOptionalParam&)dial.GetParam(step);
                param.ClearValue();
                UpdateToggleHasParamLock(*toggle);
            }
        }

        for (auto modulationSource : m_modulationSourceButtons)
        {
            if (modulationSource->GetSelectableButton().GetIsSelected())
            {
                dial.GetParam(modulationSource->GetModulationSource(m_currentTrack).GetModulations()) = 0;
                UpdateModulationSourceHasParamLock(*modulationSource);
            }
        }

        dial.UpdateOverriddenValue();

        return true;
    }

    return false;
}

void GrooveboxWindow::UpdateToggleHasParamLock(SequencerToggleWidget& toggle)
{
    bool hasOverride = false;

    for (auto dial : m_dials)
    {
        if (!dial->IsSimpleDial())
        {
            SequenceStep& step = GetCurrentSequence().GetStep(toggle.GetStepIndex());
            SynthVoiceOptionalParam& param = (SynthVoiceOptionalParam&)dial->GetParam(step);
            if (param.HasValue())
            {
                hasOverride = true;
                break;
            }
        }
    }

    toggle.GetSelectableButton().SetHasParamLock(hasOverride);
}

void GrooveboxWindow::UpdateModulationSourceHasParamLock(ModulationSourceButtonWidget& modulatonSource)
{
    bool hasOverride = false;

    for (auto dial : m_dials)
    {
        if (!dial->IsSimpleDial() && dial->GetParam(modulatonSource.GetModulationSource(m_currentTrack).GetModulations()) != 0)
        {
            hasOverride = true;
            break;
        }
    }

    modulatonSource.GetSelectableButton().SetHasParamLock(hasOverride);
}

DialWidget& GrooveboxWindow::BuildDial(
    Widget* parentWidget,
    const string& labelText,
    SynthParamGetter paramGetter,
    SynthOptionalParamGetter optionalParamGetter,
    SynthModulationShortParamGetter shortParamGetter)
{
    const SynthVoiceParam& param = paramGetter(m_sequencerStream.GetVoice(m_currentTrack).GetParams());

    DialWidget* dial = new DialWidget(
        parentWidget,
        Util::ConvertString<wstring>(labelText),
        param.Value(),
        param.MinValue(),
        param.MaxValue(),
        paramGetter,
        optionalParamGetter,
        shortParamGetter,
        *this);

    dial->SetPointAnchor(0.0f, 0.0f);
    dial->SetBoundsDelta(50.0f + DialSpacing * m_dialColumn, 50.0f + DialRowSpacing * m_dialRow, 50.0f, 50.0f);
    m_dials.push_back(dial);

    ++m_dialColumn;

    return *dial;
}

DialWidget& GrooveboxWindow::BuildSimpleDial(
    Widget* parentWidget,
    const std::string& labelText,
    SimpleSynthParamGetter paramGetter)
{
    const SynthVoiceParam& param = paramGetter();

    DialWidget* dial = new DialWidget(
        parentWidget,
        Util::ConvertString<wstring>(labelText),
        paramGetter,
        *this);

    dial->SetPointAnchor(0.0f, 0.0f);
    dial->SetBoundsDelta(50.0f + DialSpacing * m_dialColumn, 50.0f + DialRowSpacing * m_dialRow, 50.0f, 50.0f);
    m_dials.push_back(dial);

    ++m_dialColumn;

    return *dial;
}

void GrooveboxWindow::SetCurrentTrack(int trackIndex)
{
    m_currentTrack = trackIndex;
    RefreshAllDisplayedValues();
}

void GrooveboxWindow::RefreshAllDisplayedValues()
{
    for (auto dial : m_dials)
    {
        SynthVoiceParam& param = dial->IsSimpleDial()
            ? dial->GetSimpleParam()
            : dial->GetParam(m_sequencerStream.GetVoice(m_currentTrack).GetParams());

        dial->SetValue(param.Value());
        dial->UpdateOverriddenValue();
    }

    for (auto toggle : m_sequencerToggles)
    {
        int stepIndex = toggle->GetStepIndex();
        SequenceStep& step = GetCurrentSequence().GetStep(stepIndex);
        toggle->SetStepOnPipIsOn(step.GetOn());
    }

    ClearSelectedButtons();
}

SequencerStream& GrooveboxWindow::GetSequencerStream() { return m_sequencerStream; }

void GrooveboxWindow::NextDialTab()
{
    m_dialRow = 0;
    m_dialColumn = 0;
}

void GrooveboxWindow::NextDialRow()
{
    ++m_dialRow;
    m_dialColumn = 0;
}

void GrooveboxWindow::BuildModulationSourceButton(const std::array<ModulationSource*, Constants::NumTracks>& modulationSources)
{
    const float ModulationSourceSize = 100.0f;
    const float ModulationSourceMargin = 10.0f;
    const float ModulationSourceSizePlusMargin = ModulationSourceSize + ModulationSourceMargin;

    ModulationSourceButtonWidget* modulationSourceButton = new ModulationSourceButtonWidget(
        m_rootWidget,
        modulationSources,
        *this);

    modulationSourceButton->SetPointAnchor(1.0f, 0.0f);
    modulationSourceButton->SetBoundsDelta(
        -ModulationSourceSizePlusMargin,
        ModulationSourceSizePlusMargin * m_modulationSourceButtons.size() + ModulationSourceMargin,
        ModulationSourceSize,
        ModulationSourceSize);

    m_modulationSourceButtons.push_back(modulationSourceButton);
}

void GrooveboxWindow::HandleNoteStarted(NoteStartedEvent event)
{
    // cout << "Note started: " << event.Note << ", " << event.Velocity << endl;;
}

void GrooveboxWindow::HandleNoteEnded(NoteEndedEvent event)
{
    // cout << "Note ended" << endl;
}

void GrooveboxWindow::HandleStepChanged(StepChangedEvent event)
{
    if (event.Track != m_currentTrack)
    {
        return;
    }

    m_stepIndex = event.Step->GetIndex();

    Transform identity = Transform::Identity;

    for (size_t i = 0; i < m_sequencerToggles.size(); ++i)
    {
        m_sequencerToggles[i]->SetIsCurrentStepPipIsOn(i == m_stepIndex);
    }
}

void GrooveboxWindow::SaveSound(const wstring& soundName)
{
    if (soundName.length() == 0)
    {
        m_modalDialogWidget->ShowModalDialog(L"Sound name must not be empty!");
        return;
    }

    json json = m_sequencerStream.GetVoice(m_currentTrack).ToJson();

    wstringstream filePathStream;
    filePathStream << "Sounds\\" << soundName << ".json";

    Disk::CreateDirIfNotExists("Sounds");
    Disk::WriteToFile(filePathStream.str(), json.dump(4));
}

void GrooveboxWindow::LoadSound(const wstring& soundName)
{
    if (soundName.length() == 0)
    {
        m_modalDialogWidget->ShowModalDialog(L"Sound name must not be empty!");
        return;
    }

    wstringstream filePathStream;
    filePathStream << "Sounds\\" << soundName << ".json";

    string jsonString = Disk::ReadFromFile(filePathStream.str());
    json json = json::parse(jsonString);

    m_sequencerStream.GetVoice(m_currentTrack).LoadFromJson(json);
}

Sequence& GrooveboxWindow::GetCurrentSequence()
{
    return m_sequences[m_currentTrack];
}