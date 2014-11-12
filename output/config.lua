config_table = {
	window_name = "example",
	fullscreen = 1,
	window_border = 1,
	window_x = 0,
	window_y = 0,
	bpp = 24,
	resolution_w = 1300,
	resolution_h = 1000,
	doublebuffer = 1,
	
	sensitivity = vec2(2.5, 2.5),
	
	server_address = "127.0.0.1",
	server_port = 27014,
	
	nickname = "Daedalus",
	
	multiple_clients_view = 0,
	divergence_radius = 1,
	
	tickrate = 60,
	
	simulate_lag = 0,
	packet_loss = 0.00,
	min_latency = 50,
	jitter = 0
}

if config_table.fullscreen == 1 then
	config_table.resolution_w = get_display().w
	config_table.resolution_h = get_display().h
end

global_gl_window = glwindow()

local borders_type = glwindow.ALL_WINDOW_ELEMENTS

if config_table.window_border == 0 or config_table.fullscreen then
	borders_type = 0
end

global_gl_window:create(
	rect_xywh_i(config_table.window_x, 
				config_table.window_y, 
				config_table.resolution_w, 
				config_table.resolution_h), 
	borders_type, 
	config_table.window_name, 
	config_table.doublebuffer, 
	config_table.bpp)
	
global_gl_window:vsync(0)

--set_cursor_visible(0)
framework_set_current_window(global_gl_window)
