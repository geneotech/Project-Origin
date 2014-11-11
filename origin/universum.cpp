#include "stdafx.h"
#include <cmath>
#include "universum.h"

#include "game_framework/systems/render_system.h"

namespace origin {
	universum::universum(augs::rects::wh<int> size) : size(size) {
		space.resize(size.area());
	}

	void universum::simulate() {
		view_buffer.clear();
		view_buffer.resize(particles.size());

		for (size_t i = 0; i < particles.size(); ++i) {
			auto& p = particles[i];
			
			p.pos += p.vel;
			p.pos.x = fmod(p.pos.x, size.w);
			p.pos.y = fmod(p.pos.y, size.h);

			view_buffer[i] = p;
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
	}

	universum::slot& universum::get_slot(int x, int y) {
		return space[y*size.w + x];
	}

}