﻿#include "GuiMenu.hpp"
#include "GuiEngine.hpp"

using namespace inl::gui;

GuiMenu::GuiMenu(GuiEngine& guiEngine)
: GuiList(guiEngine), guiArrow(nullptr), guiButton(nullptr)
{
	
}

GuiMenu* GuiMenu::AddItemMenu(const std::wstring& text)
{
	// The item we will hover on
	GuiList* item = new GuiList(guiEngine);
	item->SetOrientation(eGuiOrientation::HORIZONTAL);
	item->SetBgToColor(ColorI(25, 25, 25, 255), ColorI(65,65,65, 255));

	// Add button
	GuiButton* btn = new GuiButton(guiEngine);
	btn->StretchFillParent();

	btn->SetText(text);
	btn->DisableHover();
	btn->HideBgColor();
	item->AddItem(btn);

	GuiMenu* subMenu = new GuiMenu(guiEngine);
	subMenu->SetGuiButton(btn);
	
	subMenu->OnChildAdd += [this, item, subMenu](Gui& self, ChildEvent& e)
	{
		// Add Arrow when we got submenu
		if (GetOrientation() == eGuiOrientation::VERTICAL && subMenu->GetChildren().size() == 1)
		{
			GuiButton* arrow = new GuiButton(subMenu->guiEngine);
			arrow->SetText(L"►");
			arrow->HideBgColor();
			arrow->DisableHover();
			arrow->AlignRight();
			arrow->StretchFitToContent();
			arrow->GetGuiText()->SetFontSize(8);
			arrow->AlignVerCenter();

			item->AddItem(arrow);
			subMenu->SetGuiArrow(arrow);
		}
	};

	subMenu->OnChildRemove += [this, item, subMenu](Gui& self, ChildEvent& e)
	{
		if (subMenu->GetChildren().size() == 0)
		{
			Gui* arrow = subMenu->GetGuiArrow();
			item->RemoveGui(arrow);
		}
	};

	subMenus[item] = subMenu;

	AddItem(item);

	return subMenu;
}

void GuiMenu::AddItem(Gui* menuItem)
{
	GuiList::AddItem(menuItem);

	if (GetOrientation() == eGuiOrientation::VERTICAL)
	{
		menuItem->StretchHorFillParent();
		menuItem->StretchVerFitToContent();
	}
	else
	{
		menuItem->StretchVerFillParent();
		menuItem->StretchHorFitToContent();
	}

	auto it = subMenus.find(menuItem);
	GuiMenu* menu = it == subMenus.end() ? nullptr : it->second;

	struct MenuTreeNode
	{
		Gui* item;
		GuiMenu* menu;
	};

	thread_local std::vector<MenuTreeNode> activeMenuTree;
	menuItem->OnCursorEnter += [menu](Gui& self, CursorEvent& evt)
	{
		// Case 1. It's menu ->		 close menus behind this AND open that one
		// case 2. It's not a menu-> close menus behind this

		// Close menus behind this
		auto it = activeMenuTree.begin();
		while (it != activeMenuTree.end())
		{
			MenuTreeNode& node = *it;
			
			Gui* item = node.item;
			GuiMenu* menu = node.menu;

			bool bSibling = item->IsSibling(self);

			if (bSibling)
			{
				while (it != activeMenuTree.end())
				{
					MenuTreeNode& node = *it;

					Gui* item = node.item;
					GuiMenu* menu = node.menu;

					// Unfreeze item, restore state to idle
					node.item->UnfreezeBg();

					if(node.item != &self)
						node.item->SetBgStateToIdle();

					// Close menu
					menu->RemoveFromParent();

					// Menu closed delete it from active menuTree
					it = activeMenuTree.erase(it);
				}
				break;
			}
			++it;
		}

		// If that button have menu assigned, OPEN it
		if (menu)
		{
			menu->BringToFront();

			GuiMenu& containingMenu = self.GetParent()->As<GuiMenu>();

			if (containingMenu.GetOrientation() == eGuiOrientation::HORIZONTAL)
				menu->SetPos(self.GetPosBottomLeft());
			else
				menu->SetPos(self.GetPosTopRight());

			self.FreezeBg();

			MenuTreeNode node;
			node.menu = menu;
			node.item = &self;
			activeMenuTree.push_back(node);
		}
	};

	guiEngine.OnCursorPress += [this](CursorEvent& evt)
	{
		bool bMenuHovered = false;

		for (MenuTreeNode& node : activeMenuTree)
		{
			bMenuHovered |= node.item->IsCursorInside();

			if (bMenuHovered)
				break;

			bMenuHovered |= node.menu->IsCursorInside();

			if (bMenuHovered)
				break;
		}

		// Close all menus !
		if (!bMenuHovered && activeMenuTree.size() > 0)
		{
			for (MenuTreeNode& node : activeMenuTree)
			{
				node.menu->RemoveFromParent();
				node.item->UnfreezeBg();
				node.item->SetBgStateToIdle();
			}
		}	
	};
}