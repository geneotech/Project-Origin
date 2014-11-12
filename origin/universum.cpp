#include "stdafx.h"
#include <cmath>
#include "universum.h"

#include "game_framework/systems/render_system.h"

namespace origin {
	universum::universum(augs::rects::wh<int> size) : size(size) {
		space.resize(size.area());
	}

	void universum::add_static(const particle& p) {
		static_particles.push_back(p);
		get_slot(int(p.pos.x), int(p.pos.y)).is_static = true;
		get_slot(int(p.pos.x), int(p.pos.y)).active_index = static_particles.size()-1;

		static_view_buffer.push_back(p);
	}

	void universum::simulate() {
		view_buffer.clear();
		view_buffer.resize(particles.size());

		for (size_t i = 0; i < particles.size(); ++i) {
			auto& p = particles[i];

			//auto secs = color_timer.get<std::chrono::seconds>();
			//auto col_val = (int(color_timer.get<std::chrono::seconds>() * 255) % 255);
			//
			//p.color.g = int(secs) % 2 ? col_val : 255 - col_val;

			auto& x = p.pos.x;
			auto& y = p.pos.y;
			slot& old_slot = get_slot(int(x), int(y));

			p.pos += p.vel;
			x = x < 0 ? size.w + x - 1 : (x >= size.w ? x - size.w : x);
			y = y < 0 ? size.h + y - 1 : (y >= size.h ? y - size.h : y);

			old_slot.occupied = false;
			view_buffer[i] = p;
		}
		
		// resolve collisions

		for (size_t i = 0; i < particles.size(); ++i) {
			auto& p = particles[i];
			auto& x = p.pos.x;
			auto& y = p.pos.y;

			slot& new_slot = get_slot(int(x), int(y));

			if (new_slot.is_static) {
				p.vel = -p.vel;
			}
			else {
				if (new_slot.occupied) {
					auto& p2 = particles[new_slot.active_index];
					auto p1_vel = p.vel;

					p.vel = (p.vel * (p.mass - p2.mass) + (2 * p2.mass * p2.vel)) / (p.mass + p2.mass);
					p2.vel = (p2.vel * (p2.mass - p.mass) + (2 * p.mass * p1_vel)) / (p2.mass + p.mass);

					new_slot.occupied = false;
				}
				else {
					new_slot.occupied = true;
				}
				new_slot.active_index = i;
			}
		}
	}

	void universum::init_render_state() {
		glGenBuffers(1, &point_buffer);
		glBindBuffer(GL_ARRAY_BUFFER, point_buffer);

		glEnableVertexAttribArray(render_system::VERTEX_ATTRIBUTES::POSITION);
		glDisableVertexAttribArray(render_system::VERTEX_ATTRIBUTES::TEXCOORD);
		glEnableVertexAttribArray(render_system::VERTEX_ATTRIBUTES::COLOR);

		glVertexAttribPointer(render_system::VERTEX_ATTRIBUTES::POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(view_particle), 0);
		//glVertexAttribPointer(VERTEX_ATTRIBUTES::TEXCOORD, 2, GL_FLOAT, GL_FALSE, sizeof(resources::vertex), (char*) (sizeof(float) * 2));
		glVertexAttribPointer(render_system::VERTEX_ATTRIBUTES::COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(view_particle), (char*) (sizeof(float) * 2));
	}

	void universum::render() {
		glBufferData(GL_ARRAY_BUFFER, sizeof(view_particle) * view_buffer.size(), view_buffer.data(), GL_STREAM_DRAW);
		glDrawArrays(GL_POINTS, 0, view_buffer.size());
		glBufferData(GL_ARRAY_BUFFER, sizeof(view_particle) * static_view_buffer.size(), static_view_buffer.data(), GL_STREAM_DRAW);
		glDrawArrays(GL_POINTS, 0, static_view_buffer.size());
	}

	universum::slot& universum::get_slot(int x, int y) {
		return space[y*size.w + x];
	}
}