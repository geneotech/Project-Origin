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

universe.gravity = vec2(0, 0.001)

for i=1, 80 do
for j=1, 40 do
	universe.particles:add(atom {
				pos = vec2(650 + 0.5+i, config_table.resolution_h - 50 + j + 0.5 - 100),
				color = rgba(255, 0, 0, 255),
				mass = 1000,
				gravity_mult = 1,
				restitution = 10,
				vel = vec2(-0.9, -0.1)
			})
end
end
		
for i=1, 150 do
	for j=1, 150 do
		universe.particles:add(atom {
			pos = vec2(250 + 20 + i + 0.5,  config_table.resolution_h - 20 - 151 + j),
			color = rgba(105+i, 105+j, 5+i+j, 255),
			mass = 1,
			gravity_mult = 1,
			restitution = 1
		})
	end
		--universe:add_static(atom {
		--	pos = vec2(config_table.resolution_w/2 + i, config_table.resolution_h/2 + j),
		--	color = rgba(0, 255-i*j/40000*255-j+i, i*j/40000*255+j-i, 255),
		--	mass = 1
		--})
		
end




for i=20, 1200 do
	universe:add_static(atom {
		pos = vec2(i, config_table.resolution_h - 20),
		color = rgba(255, 255, 255, 255)
	})
end

	
	
for i=1, 1 do
	 --for j=1, 1 do
	 --	universe.particles:add(atom {
	 --		pos = vec2(config_table.resolution_w/2 + 600 + j, config_table.resolution_h/2 + i + 250),
	 --		color = rgba(255,  0, 0, 255),
	 --		vel = vec2(-randval(0.6, 0.6), -randval(0.016, 0.016)),
	 --		mass = 15,
	--		restitution = 1,
	--		gravity_mult = 1
	 --	})
	 --end
end

--for i=1, 1 do	
--	 for j=1, 1 do
--	 	universe.particles:add(atom {
--	 		pos = vec2(config_table.resolution_w/2 - 600 + j, config_table.resolution_h/2 + i + 350),
--	 		color = rgba(255,  0, 0, 255),
--	 		vel = vec2(randval(0.9, 0.9), randval(0.016, 0.016)),
--	 		mass = 15,
--			restitution = 1,
--			gravity_mult = 1
--	 	})
--	 end
--end


universum_camera = create_world_camera_entity(world, function() 
	universe:render()
end)
--universum_camera.script:set_zoom_level(-980)
universum_camera.transform.current.pos = vec2(-200, -200)

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
	--universe.particles:at(randval(0, 200*200-1)).vel = vec2(randval(-7.0, 7), randval(-7.0, 7))
	world:render()
	
	if call_once_after_loop ~= nil then
		call_once_after_loop()
		call_once_after_loop = nil
	end
	
	global_gl_window:swap_buffers()
end

global_logfile:close()