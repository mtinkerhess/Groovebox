#include "ModulationSourceButtonWidget.h"

#include "GrooveboxWindow/GrooveboxWindow.h"
#include "ModulationSource.h"
#include "UI/LabelWidget.h"
#include "UI/SelectableButtonWidget.h"

#include <string>

using namespace std;

ModulationSourceButtonWidget::ModulationSourceButtonWidget(Widget* parent, const std::array<ModulationSource*, Constants::NumTracks>& modulationSources, GrooveboxWindow& window)
	: Widget(parent),
	m_modulationSources(modulationSources),
	m_window(window)
{
	m_selectableButton = new SelectableButtonWidget(
		this,
		[this](int mouseButton) { return m_window.ModulationSourceButtonClicked(*this, mouseButton); },
		"Button_Selected",
		Util::ConvertString<wstring>(m_modulationSources[0]->GetName()));

	m_selectableButton->GetButton().GetLabel().SetColor(sf::Color::Black);
}