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


		//void set_velocity(augs::vec2<> fraction) {
		//	int speed = 1 / fraction.length();
		//	fraction *= speed;
		//	vel = int();
		//	motion_substeps = 0;
		//}
		//float mass;
	};
}