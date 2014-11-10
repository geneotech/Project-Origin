global_logfile = io.open("client_logfile.txt", "w")

dofile "config.lua"

ENGINE_DIRECTORY = "..\\..\\Augmentations\\scripts\\"
dofile (ENGINE_DIRECTORY .. "load_libraries.lua")
dofile "input.lua"

universum = world_class:create()
SHOULD_QUIT_FLAG = false

main_input_routine = universum:create_entity {
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

universum.input_system:add_context(main_input_context)

while not SHOULD_QUIT_FLAG do
	GL.glClear(GL.GL_COLOR_BUFFER_BIT)
	
	universum:handle_input()
	universum:handle_physics()
	universum:process_all_systems()
	universum:consume_events()
	universum:render()
	
	if call_once_after_loop ~= nil then
		call_once_after_loop()
		call_once_after_loop = nil
	end
	
	global_gl_window:swap_buffers()
end

global_logfile:close()