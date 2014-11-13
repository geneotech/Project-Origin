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
			old_slot.occupied = false;

			p.vel += p.gravity_mult * gravity;
			
			if (p.vel.length_sq() > 1.f)
				p.vel.normalize();
			
			p.last_pos = p.pos;
			p.pos += p.vel;
			x = x < 0 ? size.w + x - 1 : (x >= size.w ? x - size.w : x);
			y = y < 0 ? size.h + y - 1 : (y >= size.h ? y - size.h : y);

			view_buffer[i] = p;

			p.resolved = 0;
		}
		
		// resolve collisions

		for (size_t i = 0; i < particles.size(); ++i) {
			auto& p = particles[i];

			auto& x = p.pos.x;
			auto& y = p.pos.y;

			slot& new_slot = get_slot(int(x), int(y));

			if (new_slot.is_static) {
				p.vel = -p.vel * p.restitution;
				//p.pos += p.vel;
			}
			else {
				if (new_slot.occupied) {
					auto& p2 = particles[new_slot.active_index];
					auto p1_vel = p.vel;
					auto p2_vel = p2.vel;
					
					auto p1_pos = p.pos;
					auto p2_pos = p2.pos;

					/* reposition the one with bigger velocity */
					if (p1_vel.length_sq() > p2_vel.length_sq())
						p.pos = p.last_pos;
					else
						p2.pos = p2.last_pos;

					// if they are coming towards each other
					//if ((p1_pos - p2_pos).length_sq() > ((p1_pos + p1_vel) - (p2_pos + p2_vel)).length_sq()) {
						//p.vel = (p.vel * (p.mass - p2.mass) + (2 * p2.mass * p2.vel)) / (p.mass + p2.mass);
						//p2.vel = (p2.vel * (p2.mass - p.mass) + (2 * p.mass * p1_vel)) / (p2.mass + p.mass);
						// * p2.mass * (p2.vel - p.vel) + (p.mass * p.vel) + (p2.mass * p2.vel))
						//if (!p.resolved) 
						p.vel = (p.restitution * p2.mass * (p2.vel - p.vel) + p.mass * p.vel + p2.mass * p2.vel);
						p.vel *= (1 / (p.mass + p2.mass));

						//if (!p2.resolved) 
						p2.vel = (p2.restitution * p.mass * (p1_vel - p2.vel) + p2.mass * p2.vel + p.mass * p1_vel);
						p2.vel *= (1 / (p2.mass + p.mass));

						p.resolved = true;
						p2.resolved = true;
						new_slot.occupied = false;
					//}
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