#pragma once

#include "Constants.h"
#include "UI/Widget.h"

#include <array>

class GrooveboxWindow;
class ModulationSource;
class SelectableButtonWidget;

class ModulationSourceButtonWidget : public Widget
{
	std::array<ModulationSource*, Constants::NumTracks> m_modulationSources{};
	GrooveboxWindow& m_window;
	SelectableButtonWidget* m_selectableButton;

public:
	ModulationSourceButtonWidget(Widget* parent, const std::array<ModulationSource*, Constants::NumTracks>& modulationSources, GrooveboxWindow& window);

	ModulationSource& GetModulationSource(int trackIndex)
	{
		return *m_modulationSources[trackIndex];
	}

	SelectableButtonWidget& GetSelectableButton() { return *m_selectableButton; }
};