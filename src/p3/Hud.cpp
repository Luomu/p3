#include "p3/Hud.h"
#include "p3/CoreComponents.h"

namespace p3
{
Hud::Hud(UI::Context* ui, Entity observer)
{
	m_observer = observer;

	auto layer = ui->NewLayer();
	auto label = ui->Label("Gronk");
	auto align = ui->Align(UI::Align::LEFT);
	auto box = ui->VBox();
	align->SetInnerWidget(box);
	layer->SetInnerWidget(align);

	m_velocity = label;
	m_setSpeed = ui->Label("1000 m/s");
	m_location = ui->Label("Cheeseland");

	box->PackEnd(m_velocity)
	->PackEnd(m_setSpeed)
	->PackEnd(m_location);
}

void Hud::Update(double dt)
{
	if (!m_observer.valid()) return;

	auto dc = m_observer.component<DynamicsComponent>();
	if (!dc) return;

	m_velocity->SetText(stringf("%0{f.0} m/s", dc->vel.Length()));

	auto pic = m_observer.component<PlayerInputComponent>();
	if (pic)
		m_setSpeed->SetText(stringf("%0{f.0} m/s", pic->setSpeed));

	auto fc = m_observer.component<FrameComponent>();
	if (fc)
		m_location->SetText(fc->frame->GetLabel());
}
}
