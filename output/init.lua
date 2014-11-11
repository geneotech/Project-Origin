global_logfile = io.open("client_logfile.txt", "w")

dofile "config.lua"

ENGINE_DIRECTORY = "..\\..\\Augmentations\\scripts\\"
dofile (ENGINE_DIRECTORY .. "load_libraries.lua")
dofile "input.lua"
dofile "camera.lua"

world = world_class:create()

universe = universum(rect_wh_i(config_table.resolution_w, config_table.resolution_h))
universe:init_render_state()

function atom(entry)
	local new_particle = origin_particle()
	rewrite(new_particle, entry)
	return new_particle
end

for i=1, 100 do
	for j=1, 100 do
		universe.particles:add(atom {
			pos = vec2(config_table.resolution_w/2 + i, config_table.resolution_h/2 + j),
			color = rgba(255 - i - j, 255 - i - j, 255 - i - j, 255),
			vel = vec2(randval(-0.2, 0.2), randval(-0.2, 0.2))
		})
	end
end

universum_camera = create_world_camera_entity(world, function() 
	universe:render()
end)

SHOULD_QUIT_FLAG = false

main_input_routine = world:create_entity {
	input = {
		custom_intents.QUIT
	},
	
	script = {
		intent_message = function(self, message)
			if message.intent == custom_intents.QUIT then
				SHOULD_QUIT_FLAG = true
			end
		end
	}
}

GL.glClearColor(0, 0, 0, 0)

world.input_system:add_context(main_input_context)

while not SHOULD_QUIT_FLAG do
	GL.glClear(GL.GL_COLOR_BUFFER_BIT)
	
	world:handle_input()
	world:handle_physics()
	world:process_all_systems()
	world:consume_events()
	
	universe:simulate()
	world:render()
	
	if call_once_after_loop ~= nil then
		call_once_after_loop()
		call_once_after_loop = nil
	end
	
	global_gl_window:swap_buffers()
end

global_logfile:close()