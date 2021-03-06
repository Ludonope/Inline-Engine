#include "GuiButton.hpp"

using namespace inl::gui;

GuiButton::GuiButton(GuiEngine& guiEngine)
:Gui(guiEngine)
{
	text = AddGui<GuiText>();
	text->StretchFitToContent();
	text->AlignLeft();
	text->AlignVerCenter();
	text->DisableHover();
}

GuiButton& GuiButton::operator = (const GuiButton& other)
{
	Gui::operator = (other);

	text = Copy(other.text);

	return *this;
}

void GuiButton::SetText(const std::wstring& str)
{
	text->SetText(str);
}

void GuiButton::SetText(const std::string& str)
{
	text->SetText(str);
}