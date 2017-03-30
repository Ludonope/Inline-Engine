#pragma once
#include <BaseLibrary\Common_tmp.hpp>
#include "GuiEvent.h"

// TMP HEKK (REMOVE)
#include <BaseLibrary\Platform\Window.hpp>
#define min(a,b) a < b ? a : b
#define max(a,b) a > b ? a : b
#include <gdiplus.h>
#undef min
#undef max
// TMP HEKK END

#include <unordered_map>

class GuiEngine;
class GuiPlane;
class GuiText;
class GuiButton;
class GuiList;
class GuiSlider;

class GuiControl
{
public:
	GuiControl(GuiEngine* guiEngine);

	~GuiControl() { Clear(); }
	void Clear();
	
	// Important to implement in derived classes
	virtual GuiControl& operator = (const GuiControl& other);
	virtual GuiControl* Clone() const { return new GuiControl(*this); }

	template<class T>
	T* Add();

	void		Add(GuiControl* child);
	GuiPlane*	AddPlane();
	GuiText*	AddText();
	GuiButton*	AddButton();
	GuiList*	AddList();

	bool RemoveChild(GuiControl* child);
	bool Remove();

	void TraverseTowardParents(const std::function<void(GuiControl*)>& fn);

	void Move(float dx, float dy);
	void Move(const vec2& delta) { Move(delta.x, delta.y); }

	GuiPlane*  AsPlane()  { return (GuiPlane*)this; }
	GuiText*   AsText()   { return (GuiText*)this; }
	GuiButton* AsButton() { return (GuiButton*)this; }
	GuiList*   AsList()   { return (GuiList*)this; }
	GuiSlider* AsSlider() { return (GuiSlider*)this; }

	void SetRect(float x, float y, float width, float height);
	void SetRect(const Rect<float>& rect)	{ SetRect(rect.x, rect.y, rect.width, rect.height); }
	void SetName(const std::wstring& str)	{ name = str; }
	void SetName(const std::string& str)	{ SetName(std::wstring(str.begin(), str.end())); }
	void SetContextMenu(GuiControl* c)		{ contextMenu = c; }
	void SetPos(const vec2& p)				{ SetPos(p.x, p.y); }
	void SetPos(float x, float y)			{ SetRect(x, y, size.x, size.y); }
	void SetCenterPos(float x, float y)		{ SetPos(x - GetHalfWidth(), y + GetHalfHeight()); }
	void SetPosX(float x)					{ SetRect(x, pos.y, size.x, size.y); }
	void SetPosY(float y)					{ SetRect(pos.x, y, size.x, size.y); }
	void SetSize(const vec2& s)				{ SetSize(s.x, s.y); }
	void SetSize(float width, float height) { SetRect(pos.x, pos.y, width, height); }
	void SetWidth(float w)					{ SetSize(vec2(w, size.y)); }
	void SetHeight(float h)					{ SetSize(vec2(size.x, h)); }

	void SetEventPropagationPolicy(eEventPropagationPolicy e) { eventPropagationPolicy = e; }

	float GetClientCursorPosX();
	float GetClientCursorPosY();
	float GetPosX()		  { return pos.x; }
	float GetPosY()		  { return pos.y; }
	const vec2& GetPos()  { return pos; }
	float GetCenterPosY() { return pos.y - GetHalfHeight(); }
	float GetCenterPos()  { return pos.x + GetHalfWidth(); }
	const vec2& GetSize() { return size; }
	float GetWidth()	  { return size.x; }
	float GetHeight()	  { return size.y; }
	float GetHalfWidth()  { return GetWidth() * 0.5f; }
	float GetHalfHeight() { return GetHeight() * 0.5f; }
	Rect<float> GetRect() { return Rect<float>(pos.x, pos.y, size.x, size.y); }
	GuiControl* GetParent() { return parent; }
	GuiControl* GetContextMenu() { return contextMenu; }
	const std::vector<GuiControl*>& GetChildren() { return children; }

	eEventPropagationPolicy GetEventPropagationPolicy() { return eventPropagationPolicy; }

	bool IsPointInside(ivec2 pt) { return GetRect().IsPointInside(pt); }

protected:
	std::wstring name;
	vec2 pos;
	vec2 size;
	GuiControl* parent;
	std::vector<GuiControl*> children;
	std::unordered_map<GuiControl*, size_t> childrenIndices; // For optimizing (Add & Remove) functions

	eEventPropagationPolicy eventPropagationPolicy;

	// Layering (Our neighbours in the tree hierarchy)
	GuiControl* front;
	GuiControl* back;

	// Context menu
	GuiControl* contextMenu;

	GuiEngine* guiEngine;

public:
	Delegate<void(GuiControl* self, CursorEvent& evt)> onMouseClick;
	Delegate<void(GuiControl* self, CursorEvent& evt)> onMousePress;
	Delegate<void(GuiControl* self, CursorEvent& evt)> onMouseRelease;
	Delegate<void(GuiControl* self, CursorEvent& evt)> onMouseMove;
	Delegate<void(GuiControl* self, CursorEvent& evt)> onMouseEnter;
	Delegate<void(GuiControl* self, CursorEvent& evt)> onMouseLeave;
	Delegate<void(GuiControl* self, CursorEvent& evt)> onMouseHover;

	Delegate<void(GuiControl* self, Rect<float>& rect)> onTransformChange;
	Delegate<void(GuiControl* self, Rect<float>& rect)> onParentTransformChange;

	Delegate<void(GuiControl* self, GuiControl* parent)> onParentChange;

	Delegate<void(GuiControl* self, HDC dc, Gdiplus::Graphics* graphics)> onPaint;
	Delegate<void(GuiControl* self, float deltaTime)> onUpdate;
};

inline GuiControl::GuiControl(GuiEngine* guiEngine)
:guiEngine(guiEngine), pos(0, 0), eventPropagationPolicy(eEventPropagationPolicy::PROCESS), size(60,20), parent(nullptr), front(nullptr), back(nullptr), contextMenu(nullptr)
{

}

inline void GuiControl::Clear()
{
	front = nullptr;
	back = nullptr;
	parent = nullptr;

	for (GuiControl* c : children)
		delete c;

	children.clear();
	childrenIndices.clear();
}

inline GuiControl& GuiControl::operator = (const GuiControl& other)
{
	Clear();

	guiEngine = other.guiEngine;
	pos = other.pos;
	size = other.size;
	name = other.name;
	onMouseClick = other.onMouseClick;
	onMousePress = other.onMousePress;
	onMouseRelease = other.onMouseRelease;
	onMouseMove = other.onMouseMove;
	onMouseEnter = other.onMouseEnter;
	onMouseLeave = other.onMouseLeave;
	onMouseHover = other.onMouseHover;
	onTransformChange = other.onTransformChange;
	onParentTransformChange = other.onParentTransformChange;
	onPaint = other.onPaint;
	onUpdate = other.onUpdate;

	// Context menu
	if(other.contextMenu)
		contextMenu = other.contextMenu->Clone();

	for (GuiControl* child : other.children)
		Add(child->Clone());

	return *this;
}

template<class T>
T* GuiControl::Add()
{
	T* child = new T(guiEngine);
	Add(child);
	return child;
}

inline void GuiControl::Add(GuiControl* child)
{
	child->parent = this;

	if (children.size() != 0)
	{
		GuiControl* lastControl = children[children.size() - 1];
		child->back = lastControl;
		lastControl->front = child;
	}

	childrenIndices.insert(std::make_pair(child, children.size()));
	children.push_back(child);

	child->onParentChange(child, this);
}

inline bool GuiControl::RemoveChild(GuiControl* child)
{
	auto it = childrenIndices.find(child);
	if (it != childrenIndices.end())
	{
		child->parent = nullptr;

		if (child->front)
			child->front->back = nullptr;

		if (child->back)
			child->back->front = nullptr;

		child->front = nullptr;
		child->back = nullptr;

		children.erase(children.begin() + it->second);
		childrenIndices.erase(it);
		return true;
	}
	else
	{
		return false;
	}
}

inline bool GuiControl::Remove()
{
	if (parent)
		return parent->RemoveChild(this);

	return false;
}

inline void GuiControl::TraverseTowardParents(const std::function<void(GuiControl*)>& fn)
{
	// Terminate recursive call
	if (eventPropagationPolicy == eEventPropagationPolicy::STOP)
		return;

	// Process fn for this control
	if (eventPropagationPolicy == eEventPropagationPolicy::PROCESS || eventPropagationPolicy == eEventPropagationPolicy::PROCESS_STOP)
		fn(this);

	// Continue recursive calls
	if (eventPropagationPolicy == eEventPropagationPolicy::PROCESS || eventPropagationPolicy == eEventPropagationPolicy::AVOID)
	{
		if (back)
			back->TraverseTowardParents(fn);
		else if (parent)
			fn(parent);// parent->TraverseTowardParents(fn);
	}
}

inline void GuiControl::Move(float dx, float dy)
{
	pos.x += dx;
	pos.y += dy;

	for (GuiControl* child : children)
		child->Move(dx, dy);
}

inline void GuiControl::SetRect(float x, float y, float width, float height)
{
	pos.x = x;
	pos.y = y;
	size.x = width;
	size.y = height;

	Rect<float> rect = GetRect();
	onTransformChange(this, rect);

	for (GuiControl* child : children)
		child->onParentTransformChange(child, rect);
}