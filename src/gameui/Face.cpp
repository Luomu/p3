// Copyright © 2008-2014 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#include "Face.h"
#include "FileSystem.h"
#include "SDLWrappers.h"
#include "graphics/TextureBuilder.h"

using namespace UI;

namespace GameUI {

static const Uint32 FACE_WIDTH = 295;
static const Uint32 FACE_HEIGHT = 285;

RefCountedPtr<Graphics::Material> Face::s_material;

Face::Face(Context *context, Uint32 flags, Uint32 seed) : Single(context), m_preferredSize(INT_MAX)
{
	if (!seed) seed = time(0);

	m_flags = flags;
	m_seed = seed;

	m_preferredSize = UI::Point(FACE_WIDTH, FACE_HEIGHT);
	SetSizeControlFlags(UI::Widget::PRESERVE_ASPECT);
}

UI::Point Face::PreferredSize() {
	return m_preferredSize;
}

void Face::Layout()
{
	Point size(GetSize());
	Point activeArea(std::min(size.x, size.y));
	Point activeOffset(std::max(0, (size.x-activeArea.x)/2), std::max(0, (size.y-activeArea.y)/2));
	SetActiveArea(activeArea, activeOffset);

	Widget *innerWidget = GetInnerWidget();
	if (!innerWidget) return;
	SetWidgetDimensions(innerWidget, activeOffset, activeArea);
	innerWidget->Layout();
}

void Face::Draw()
{
	Single::Draw();
}

Face *Face::SetHeightLines(Uint32 lines)
{
	const Text::TextureFont *font = GetContext()->GetFont(GetFont()).Get();
	const float height = font->GetHeight() * lines;
	m_preferredSize = UI::Point(height * float(FACE_WIDTH) / float(FACE_HEIGHT), height);
	GetContext()->RequestLayout();
	return this;
}

}
