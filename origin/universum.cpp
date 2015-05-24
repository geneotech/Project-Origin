#include "stdafx.h"
#include <cmath>
#include "universum.h"

#include "game_framework/systems/render_system.h"

void SaveBitmapToFile(BYTE* pBitmapBits,
	LONG lWidth,
	LONG lHeight,
	WORD wBitsPerPixel,
	const unsigned long& padding_size,
	LPCTSTR lpszFileName);

namespace origin {
	universum::universum(augs::rects::wh<int> size) : size(size) {
		space.resize(size.area());
	}

	void universum::add_static(const particle& p) {
		static_particles.push_back(p);
		//get_slot(int(p.pos.x), int(p.pos.y)).is_static = true;
		//get_slot(int(p.pos.x), int(p.pos.y)).active_index = static_particles.size()-1;

		static_view_buffer.push_back(p);
	}
	
	void universum::clear_static() {
		static_view_buffer.clear();
		static_particles.clear();
	}
	
	void universum::rescale_static(float percentage) {


	}

	void universum::save_ss() {
		vec2<int> upper_left(std::numeric_limits<int>::max(), std::numeric_limits<int>::max());
		vec2<int> bottom_right(std::numeric_limits<int>::min(), std::numeric_limits<int>::min());

		for (auto& p : static_view_buffer) {
			upper_left.x = std::min(upper_left.x, int(p.pos.x));
			upper_left.y = std::min(upper_left.y, int(p.pos.y));
			bottom_right.x = std::max(bottom_right.x, int(p.pos.x));
			bottom_right.y = std::max(bottom_right.y, int(p.pos.y));
		}
		
		auto w = (bottom_right.x - upper_left.x)+4 - (bottom_right.x - upper_left.x)%4 + 4;
		auto h = (bottom_right.y - upper_left.y) + 4;
		auto size = w * h;

		BYTE* bytes = new BYTE[size*3 * sizeof(BYTE)];
		memset(bytes, 0, size*sizeof(BYTE)*3);

		for (auto& p : static_view_buffer) {
			int x = int(p.pos.x) - upper_left.x;
			int y = int(p.pos.y) - upper_left.y;

			bytes[(y * w + x) * 3] = p.color.r;
			bytes[(y * w + x) * 3 + 1] = p.color.g;
			bytes[(y * w + x) * 3 + 2] = p.color.b;
		}

		SaveBitmapToFile(bytes, w, h, 24, 0, L"savedbmp.bmp");

		return;
	}

	void universum::simulate() {
		view_buffer.clear();
		view_buffer.resize(particles.size());
		auto delta = timer.extract<std::chrono::seconds>();

		for (size_t i = 0; i < particles.size(); ++i) {
			auto& p = particles[i];

			//auto secs = color_timer.get<std::chrono::seconds>();
			//auto col_val = (int(color_timer.get<std::chrono::seconds>() * 255) % 255);
			//
			//p.color.g = int(secs) % 2 ? col_val : 255 - col_val;

			slot& new_slot = get_slot(int(p.pos.x), int(p.pos.y));
			new_slot.occupied = true;
			new_slot.active_index = i;

			auto& x = p.requested_pos.x;
			auto& y = p.requested_pos.y;

			p.vel += p.gravity_mult * delta * gravity;
			
			if (p.vel.length_sq() > 1.f)
				p.vel.normalize();
			
			p.requested_pos = p.pos;
			p.requested_pos += p.vel * delta;
			x = x < 0 ? size.w + x - 1 : (x >= size.w ? x - size.w : x);
			y = y < 0 ? size.h + y - 1 : (y >= size.h ? y - size.h : y);
			view_buffer[i] = p;

			p.resolved = 0;
		}
		
		// resolve collisions

		for (size_t i = 0; i < particles.size(); ++i) {
			auto& p = particles[i];

			auto& x = p.requested_pos.x;
			auto& y = p.requested_pos.y;

			slot& requested_slot = get_slot(int(x), int(y));

			if (&requested_slot == &get_slot(p.pos.x, p.pos.y)) {
				p.pos = p.requested_pos;
			}
			else if (requested_slot.is_static) {
				p.vel = -p.vel * p.restitution;
				//p.pos += p.vel;
			}
			else if (requested_slot.occupied) {
				auto& p2 = particles[requested_slot.active_index];
				auto p1_vel = p.vel;
				auto p2_vel = p2.vel;
				
				auto p1_pos = p.pos;
				auto p2_pos = p2.pos;

				// if they are coming towards each other
				if ((p1_pos - p2_pos).length_sq() > ((p1_pos + p1_vel) - (p2_pos + p2_vel)).length_sq()) {
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
				}
			}
			else {
				int i_x = int(p.pos.x);
				int i_y = int(p.pos.y);
				get_slot(i_x, i_y).occupied = false;

				p.pos = p.requested_pos;
				/* make it occupied for this collision pass already */
				requested_slot.occupied = true;
				requested_slot.active_index = i;
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

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0.0f, 1920, 1080, 0.0f, 0.0f, 1.0f);
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

void SaveBitmapToFile(BYTE* pBitmapBits,
	LONG lWidth,
	LONG lHeight,
	WORD wBitsPerPixel,
	const unsigned long& padding_size,
	LPCTSTR lpszFileName)
{
	// Some basic bitmap parameters 
	unsigned long headers_size = sizeof(BITMAPFILEHEADER) +
		sizeof(BITMAPINFOHEADER);

	unsigned long pixel_data_size = lHeight * ((lWidth * (wBitsPerPixel / 8)) + padding_size);

	BITMAPINFOHEADER bmpInfoHeader = { 0 };

	// Set the size 
	bmpInfoHeader.biSize = sizeof(BITMAPINFOHEADER);

	// Bit count 
	bmpInfoHeader.biBitCount = wBitsPerPixel;

	// Use all colors 
	bmpInfoHeader.biClrImportant = 0;

	// Use as many colors according to bits per pixel 
	bmpInfoHeader.biClrUsed = 0;

	// Store as un Compressed 
	bmpInfoHeader.biCompression = BI_RGB;

	// Set the height in pixels 
	bmpInfoHeader.biHeight = lHeight;

	// Width of the Image in pixels 
	bmpInfoHeader.biWidth = lWidth;

	// Default number of planes 
	bmpInfoHeader.biPlanes = 1;

	// Calculate the image size in bytes 
	bmpInfoHeader.biSizeImage = pixel_data_size;

	BITMAPFILEHEADER bfh = { 0 };

	// This value should be values of BM letters i.e 0x4D42 
	// 0x4D = M 0×42 = B storing in reverse order to match with endian 
	bfh.bfType = 0x4D42;
	//bfh.bfType = 'B'+('M' << 8);

	// <<8 used to shift ‘M’ to end  */ 

	// Offset to the RGBQUAD 
	bfh.bfOffBits = headers_size;

	// Total size of image including size of headers 
	bfh.bfSize = headers_size + pixel_data_size;

	// Create the file in disk to write 
	HANDLE hFile = CreateFile(lpszFileName,
		GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	// Return if error opening file 
	if (!hFile) return;

	DWORD dwWritten = 0;

	// Write the File header 
	WriteFile(hFile,
		&bfh,
		sizeof(bfh),
		&dwWritten,
		NULL);

	// Write the bitmap info header 
	WriteFile(hFile,
		&bmpInfoHeader,
		sizeof(bmpInfoHeader),
		&dwWritten,
		NULL);

	// Write the RGB Data 
	WriteFile(hFile,
		pBitmapBits,
		bmpInfoHeader.biSizeImage,
		&dwWritten,
		NULL);

	// Close the file handle 
	CloseHandle(hFile);
}