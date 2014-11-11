#pragma once
#include "stdafx.h"
#include "universum.h"
#include "utilities/lua_state_wrapper.h"
#include "misc/vector_wrapper.h"

namespace origin {
	void universum::bind(augs::lua_state_wrapper& wrapper) {
		luabind::module(wrapper.raw)[
			luabind::class_<particle>("origin_particle")
				.def(luabind::constructor<>())
				.def_readwrite("color", &particle::color)
				.def_readwrite("pos", &particle::pos)
				.def_readwrite("vel", &particle::vel)
				.def_readwrite("mass", &particle::mass),

				luabind::class_<slot>("origin_slot")
				.def(luabind::constructor<>())
				.def_readwrite("occupied", &slot::occupied)
				.def_readwrite("data", &slot::data),

				augs::misc::vector_wrapper<particle>::bind_vector("particle_vector"),

				luabind::class_<universum>("universum")
				.def(luabind::constructor<augs::rects::wh<int>>())
				.def("get_slot", &universum::get_slot)
				.def("simulate", &universum::simulate)
				.def("render", &universum::render)
				.def("init_render_state", &universum::init_render_state)
				.def_readwrite("particles", &universum::particles)
		];
	}
}