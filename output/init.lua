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

universe.gravity = vec2(0, 0.00)

start_pos = vec2_i(0,0)

switch = false
populationnum = 0

local modfu = function(a, b)
	return a - math.floor(a/b)*b
end

function populate()
	populationnum = populationnum+1
	
	turn = 1
	offset = 0
	coords = vec2 (0, 0)
	
	p = 1
	
	universe:clear_static()

	local place = function(x, y)
		
		--if modfu(p, populationnum+1) == 0 then
		if is_prime(p) then
		
			universe:add_static(atom {
				pos = vec2(start_pos.x + x, start_pos.y + y),
				color = rgba(255, 255, 255, 255)
			})
		end
		
		p = p + 1
	end

	
	for i=1,2000 do
		if turn == 1 then
			for j=1, offset do
				coords.x = coords.x + 1
				place(coords.x, coords.y)
			end
			
		elseif turn == 2 then
			for j=1, offset do
				coords.y = coords.y - 1
				place(coords.x, coords.y)
			end
		elseif turn == 3 then
			for j=1, offset do
				coords.x = coords.x - 1
				place(coords.x, coords.y)
			end
		elseif turn == 4 then
			for j=1, offset do
				coords.y = coords.y + 1
				place(coords.x, coords.y)
			end
		end
		
		turn = turn + 1
		
		if turn == 5 then
			turn = 1
			coords.x = coords.x - 1
			coords.y = coords.y + 1
			offset = offset + 2
		end
	
	end

end

populate()

universum_camera = create_world_camera_entity(world, function() 
	universe:render()
end)

universe.timestep = 1
universum_camera.script:set_zoom_level(0)
universum_camera.script.min_zoom = -1000000
universum_camera.script.max_zoom = 1000000
universum_camera.transform.current.pos = vec2(config_table.resolution_w/2, config_table.resolution_h/2)
SHOULD_QUIT_FLAG = false

main_input_routine = world:create_entity {
	input = {
		custom_intents.QUIT,
		custom_intents.INCREASE_STEP,
		custom_intents.DECREASE_STEP,
		intent_message.MOVE_FORWARD,
		intent_message.MOVE_BACKWARD,
		intent_message.MOVE_LEFT,
		intent_message.MOVE_RIGHT,
		custom_intents.SWITCH,
		custom_intents.REPOPULATE,
		custom_intents.ZOOM_CAMERA
	},
	
	script = {
		intent_message = function(self, message)
			if message.intent == custom_intents.QUIT then
				SHOULD_QUIT_FLAG = true
			end
			
			local function pzoom()
				print("zoom: " .. universum_camera.script.current_zoom_level)
				print("x: ".. universum_camera.transform.current.pos.x)
				print("y: " .. universum_camera.transform.current.pos.y)
			end
			
			if message.intent == custom_intents.INCREASE_STEP then
				universe.timestep = universe.timestep * 2
			elseif message.intent == custom_intents.DECREASE_STEP then
				universe.timestep = universe.timestep / 2
			end
			
			if message.intent == intent_message.MOVE_FORWARD then
				universum_camera.transform.current.pos.y = universum_camera.transform.current.pos.y + 10
				pzoom()
			end
			
			if message.intent == intent_message.MOVE_BACKWARD then
				universum_camera.transform.current.pos.y = universum_camera.transform.current.pos.y - 10
				pzoom()
			end
			
			if message.intent == intent_message.MOVE_LEFT then
				universum_camera.transform.current.pos.x = universum_camera.transform.current.pos.x + 10
				pzoom()
			end
			
			if message.intent == intent_message.MOVE_RIGHT then
				universum_camera.transform.current.pos.x = universum_camera.transform.current.pos.x - 10
				pzoom()
			end			

			if message.intent == custom_intents.REPOPULATE and message.state_flag then
				populate()
			end
				
			if message.intent == custom_intents.SWITCH then
				if message.state_flag then
					--switch = not switch
					--populate()
					universe:save_ss()
					print (switch)
				end
			end
			
			if message.intent == custom_intents.ZOOM_CAMERA then
				pzoom()
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