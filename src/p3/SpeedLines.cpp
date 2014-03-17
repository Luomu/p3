#include "p3/SpeedLines.h"
#include "p3/CoreComponents.h"

namespace p3
{
const float BOUNDS     = 2000.f;
const int   DEPTH      = 8;
const float SPACING    = 500.f;
const float MAX_VEL    = 100.f;

const Color DEFAULT_COLOR  = Color(255, 255, 0, 255);
const Color FAST_COLOR     = Color(0, 255, 0, 255);
const Color VERYFAST_COLOR = Color(128, 128, 128, 255);

SpeedLines::SpeedLines(Graphics::Renderer* r, Entity owner)
	: Graphic(r)
	, m_visible(true)
	, m_color(255)
	, m_lineLength(1.f)
	, m_dir(1.f)
	, m_transform(1.f)
	, m_owner(owner)
{
	m_points.reserve(DEPTH * DEPTH * DEPTH);
	for (int x = -DEPTH / 2; x < DEPTH / 2; x++) {
		for (int y = -DEPTH / 2; y < DEPTH / 2; y++) {
			for (int z = -DEPTH / 2; z < DEPTH / 2; z++) {
				m_points.push_back(vector3f(x * SPACING, y * SPACING, z * SPACING));
			}
		}
	}

	m_vertices.resize(m_points.size() * 2);
	m_vtxColors.resize(m_vertices.size());

	Graphics::RenderStateDesc rsd;
	rsd.blendMode  = Graphics::BLEND_ALPHA_ONE;
	rsd.depthTest  = true;
	rsd.depthWrite = false;
	m_renderState  = m_renderer->CreateRenderState(rsd);
}

void SpeedLines::Update(double dt)
{
	auto dc = m_owner.component<DynamicsComponent>();
	SDL_assert(dc);
	vector3f vel(dc->vel);
	const float absVel = vel.Length();

	//alter line color to give overall idea of speed
	//slow lines down at higher speeds
	float mult;
	if (absVel > 100000.f) {
		m_color = VERYFAST_COLOR;
		mult = 0.001f;
	} else if (absVel > 10000.f) {
		m_color = VERYFAST_COLOR;
		mult = 0.01f;
	} else if (absVel > 5000.f) {
		m_color = FAST_COLOR;
		mult = 0.1f;
	} else {
		m_color = DEFAULT_COLOR;
		mult = 1.f;
	}

	//rate of change (incl. time acceleration)
	float d = absVel * dt * mult;

	//don't draw when almost stopped
	if (d < 0.0001f) {
		m_visible = false;
		return;
	}
	m_visible = true;

	m_lineLength = Clamp(absVel * 0.1f, 2.f, 100.f);
	m_dir = vel.Normalized();

	vel = vel * dt * mult;

	//too fast to draw - cap
	if (d > MAX_VEL)
		vel = m_dir * MAX_VEL;

	for (Uint16 i = 0; i < m_points.size(); i++) {

		vector3f& pt = m_points[i];

		pt -= vel;

		//wrap around
		if (pt.x > BOUNDS)
			pt.x -= BOUNDS * 2.f;
		if (pt.x < -BOUNDS)
			pt.x += BOUNDS * 2.f;
		if (pt.y > BOUNDS)
			pt.y -= BOUNDS * 2.f;
		if (pt.y < -BOUNDS)
			pt.y += BOUNDS * 2.f;
		if (pt.z > BOUNDS)
			pt.z -= BOUNDS * 2.f;
		if (pt.z < -BOUNDS)
			pt.z += BOUNDS * 2.f;
	}
}

void SpeedLines::Render()
{
	if (!m_visible) return;

	const vector3f dir = m_dir * m_lineLength;

	Uint16 vtx = 0;
	for (auto it = m_points.begin(); it != m_points.end(); ++it) {
		m_vertices[vtx]   = *it - dir;
		m_vertices[vtx + 1] = *it + dir;

		//distance fade
		const Color col = Color(m_color.r, m_color.g, m_color.b, 255 * std::max(1.f - it->Length() / BOUNDS, 0.f));
		m_vtxColors[vtx]   = col;
		m_vtxColors[vtx + 1] = col;

		vtx += 2;
	}

	matrix4x4d vt = m_renderer->GetViewMatrix();
	vt.ClearToRotOnly();
	m_renderer->SetTransform(vt);
	m_renderer->DrawLines(m_vertices.size(), &m_vertices[0], &m_vtxColors[0], m_renderState);
}
}