#pragma once
#include "stdafx.h"

#include "utilities/lua_state_wrapper.h"
#include "game_framework/game_framework.h"

#include "augmentations.h"
#include "origin/universum.h"

int main() {
	framework::init();

	augs::lua_state_wrapper lua_state;
	framework::bind_whole_engine(lua_state);
	origin::universum::bind(lua_state);
	
	lua_state.dofile("init.lua");  

	//augs::window::set_display(1920, 1080, 32);
	//augs::window::glwindow myw;
	//
	//myw.create(augs::rects::xywh<int>(0, 0, 1920, 1080), 0);
	//framework::set_current_window(myw);
	//
	//origin::universum myuniv(augs::rects::wh<int>(1920, 1080));
	//myuniv.init_render_state();
	//
	//glClearColor(0, 0, 0, 1);
	//while (true) {
	//	glClear(GL_COLOR_BUFFER_BIT);
	//
	//	augs::window::event::message msg;
	//	myw.poll_events(msg);
	//
	//	if (msg == augs::window::event::key::down && myw.events.key == augs::window::event::keys::ESC)
	//		break;
	//
	//	myuniv.render();
	//	myw.swap_buffers();
	//}

	framework::deinit();
	return 0;
}