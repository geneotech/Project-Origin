#pragma once
#include "math/vec2d.h"
#include "graphics/pixel.h"

namespace origin {
	struct view_particle {
		augs::vec2<> pos;
		augs::graphics::pixel_32 color;
	};

	struct particle : view_particle {
		augs::vec2<> vel;
		float mass = 1.f;
		float gravity_mult = 0.f;
		float restitution = 0.f;
	};
}