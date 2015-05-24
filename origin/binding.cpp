#pragma once
#include "stdafx.h"
#include "universum.h"
#include "utilities/lua_state_wrapper.h"
#include "misc/vector_wrapper.h"

#include <cmath>

bool is_prime(int number)
{
	if (number<2)
		return false;
	if (number == 2)
		return true;
	if (number % 2 == 0)
		return false;

	int sq = sqrt(number);

	for (int i = 3; i <= sq; i += 2)
	{
		if (number%i == 0)
			return false;
	}
	return true;
}

namespace origin {
	void universum::bind(augs::lua_state_wrapper& wrapper) {
		luabind::module(wrapper.raw)[
			luabind::def("is_prime", is_prime),

			luabind::class_<particle>("origin_particle")
				.def(luabind::constructor<>())
				.def_readwrite("color", &particle::color)
				.def_readwrite("pos", &particle::pos)
				.def_readwrite("vel", &particle::vel)
				.def_readwrite("gravity_mult", &particle::gravity_mult)
				.def_readwrite("restitution", &particle::restitution)
				.def_readwrite("mass", &particle::mass),

				luabind::class_<slot>("origin_slot")
				.def(luabind::constructor<>())
				.def_readwrite("occupied", &slot::occupied)
				.def_readwrite("data", &slot::active_index),

				augs::misc::vector_wrapper<particle>::bind_vector("particle_vector"),

				luabind::class_<universum>("universum")
				.def(luabind::constructor<augs::rects::wh<int>>())
				.def("get_slot", &universum::get_slot)
				.def("simulate", &universum::simulate)
				.def("render", &universum::render)
				.def("add_static", &universum::add_static)
				.def("init_render_state", &universum::init_render_state)
				.def("clear_static", &universum::clear_static)
				.def("save_ss", &universum::save_ss)
				.def_readwrite("gravity", &universum::gravity)
				.def_readwrite("timestep", &universum::timestep)
				.def_readwrite("particles", &universum::particles)
				.def_readwrite("static_particles", &universum::static_particles)
		];
	}
}