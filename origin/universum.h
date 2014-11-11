#pragma once
#include <vector>
#include "math/vec2d.h"
#include "particle.h"

namespace augs {
	struct lua_state_wrapper;
}

namespace origin {
	struct universum {
		struct slot {
			bool occupied = false;
			particle data;
		};

		std::vector<slot> space;
		std::vector<particle> particles;

		std::vector<view_particle> view_buffer;

		slot& get_slot(int x, int y);

		augs::rects::wh<int> size;

		universum(augs::rects::wh<int>);

		void simulate();

		unsigned point_buffer;

		void init_render_state();
		void render();

		static void bind(augs::lua_state_wrapper& wrapper);
	};
}