#include "p3/SpaceSystems.h"
#include "p3/Space.h"
#include "p3/CoreComponents.h"

namespace p3
{
void FrameUpdateSystem::update(ent_ptr<entityx::EntityManager> em, ent_ptr<entityx::EventManager> events, double dt)
{
	for (auto entity : em->entities_with_components<FrameComponent, DynamicsComponent>()) {
		//ZZZ can_move_frame check
		//ZZZ shouldn't be limited to DynamicsComponents (think projectiles)
		//planets, stations are immmovablew

        auto fc  = entity.component<FrameComponent>();
        auto poc = entity.component<PosOrientComponent>();

		//falling out of frames
        if (fc->frame->GetRadius() < poc->pos.Length()) {
			Frame* newFrame = fc->frame->GetParent();
			if (newFrame) {
				SwitchFrame(entity, newFrame);
				return;
			}
        }

        //entering into frames
		for (Frame* child : fc->frame->GetChildren()) {
			const vector3d pos = Space::GetPosRelTo(entity, child);
			if (pos.Length() >= child->GetRadius()) continue;
			SwitchFrame(entity, child);
			break;
		}
	}
}

void FrameUpdateSystem::SwitchFrame(Entity e, Frame* newFrame)
{
	auto poc = e.component<PosOrientComponent>();
	auto dc  = e.component<DynamicsComponent>();
	auto fc  = e.component<FrameComponent>();
	Frame* oldFrame = fc->frame;
	const vector3d vel = Space::GetVelRelTo(e, newFrame); // do this first because it uses position
	const vector3d fpos = fc->frame->GetPositionRelTo(newFrame);
	const matrix3x3d forient = fc->frame->GetOrientRelTo(newFrame);
	poc->pos = forient * poc->pos + fpos;
	poc->orient = forient * poc->orient;
	dc->vel = vel + newFrame->GetStasisVelocity(poc->pos);
	fc->frame = newFrame;
	dc->externalForce = vector3d(0.0);

	//LuaEvent::Queue("onFrameChanged", this);
	//ZZZ collision geom switch does not belong here
	auto cmc = e.component<CollisionMeshComponent>();
	if (cmc) {
		oldFrame->RemoveGeom(cmc->GetGeom());
		newFrame->AddGeom(cmc->GetGeom());
	}
}
}
