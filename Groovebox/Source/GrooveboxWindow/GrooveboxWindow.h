#pragma once

#include "Constants.h"
#include "EventTypes.h"
#include "Voice/SynthParamGetters.h"

#include <SFML/Graphics.hpp>

#include <iostream>

class DialWidget;
class LayoutWidget;
class ModalDialogWidget;
class ModulationSource;
class ModulationSourceButtonWidget;
class Sequence;
class SequencerStream;
class SequencerToggleWidget;
class TabbedPanelWidget;
class Widget;

class GrooveboxWindow
{
public:
    static const int SequencerTogglesLayoutHeight = 200;
    static const int DialSpacing = 200;
    static const int DialRowSpacing = 150;

private:
	sf::RenderWindow& m_sfmlWindow;

    int m_currentTrack = 0;
    std::array<Sequence, Constants::NumTracks>& m_sequences;
    SequencerStream& m_sequencerStream;

    Widget* m_rootWidget;

    ModalDialogWidget* m_modalDialogWidget;

    NoteStartedHandler m_noteStartedHandler;
    NoteEndedHandler m_noteEndedHandler;
    StepChangedHandler m_stepChangedHandler;

    int m_stepIndex = 0;
    
    LayoutWidget* m_sequencerTogglesLayout;
    std::vector<SequencerToggleWidget*> m_sequencerToggles;

    TabbedPanelWidget* m_tabbedPanel;

    std::vector<DialWidget*> m_dials;
    int m_dialRow = 0;
    int m_dialColumn = 0;
    std::vector<ModulationSourceButtonWidget*> m_modulationSourceButtons;

public:
    GrooveboxWindow(sf::RenderWindow& sfmlWindow, SequencerStream& sequencerStream, std::array<Sequence, Constants::NumTracks>& sequences);
    
    ~GrooveboxWindow();

    void Tick(double deltaSeconds);
    
    void ProcessInput();
    
    void Update(double deltaSeconds);
    
    void Draw();
    
    bool SequencerToggleClicked(int stepIndex, int mouseButton);
    
    void ClearSelectedButtons();
    
    bool GetSequencerStepIsSelected(int stepIndex) const;
    
    void SetSequencerStepIsSelected(int stepIndex, bool isSelected);
    
    void ToggleSequencerStepIsSelected(int stepIndex);

    bool ModulationSourceButtonClicked(ModulationSourceButtonWidget& button, int mouseButton);

    void UpdateDialOverriddenValues();

    void DialValueChanged(DialWidget& dial, int deltaValue, int mouseButton);

    bool ValueIsOverridden(SynthOptionalParamGetter optionalParamGetter, SynthModulationShortParamGetter shortParamGetter, std::string& overriddenValueString);

    bool HandleDialClicked(DialWidget& dial, int mouseButton);

    void UpdateToggleHasParamLock(SequencerToggleWidget& toggle);

    void UpdateModulationSourceHasParamLock(ModulationSourceButtonWidget& modulatonSource);

    DialWidget& BuildDial(
        Widget* parentWidget,
        const std::string& labelText,
        SynthParamGetter paramGetter,
        SynthOptionalParamGetter optionalParamGetter,
        SynthModulationShortParamGetter shortParamGetter);

    DialWidget& BuildSimpleDial(
        Widget* parentWidget,
        const std::string& labelText,
        SimpleSynthParamGetter paramGetter);

    void SetCurrentTrack(int trackIndex);

    void RefreshAllDisplayedValues();

    SequencerStream& GetSequencerStream();

private:

    void NextDialTab();

    void NextDialRow();

    void BuildModulationSourceButton(const std::array<ModulationSource*, Constants::NumTracks>& modulationSources);

    void HandleNoteStarted(NoteStartedEvent event);

    void HandleNoteEnded(NoteEndedEvent event);

    void HandleStepChanged(StepChangedEvent event);

    void SaveSound(const std::wstring& soundName);

    void LoadSound(const std::wstring& soundName);

    Sequence& GetCurrentSequence();
};