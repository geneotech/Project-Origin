function create_world_camera_entity(owner_world, drawing_routine)
	local camera_archetype = {
		transform = {
			pos = vec2(),
			rotation = 0
		},
	
		camera = {
			enabled = true,
			
			layer = 0, -- 0 = topmost
			mask = render_component.WORLD,
			
			enable_smoothing = true,
			smoothing_average_factor = 0.5,
			averages_per_sec = 20,
			
			crosshair = nil,
			player = nil,
		
			orbit_mode = camera_component.LOOK,
			max_look_expand = vec2(config_table.resolution_w/2, config_table.resolution_h/2),
			angled_look_length = 10
		},
		
		chase = {
			relative = false
			--offset = vec2(config_table.resolution_w/(-2), config_table.resolution_h/(-2))
		}
	}
	
	local vertex_shader_code = [[
	#version 330
	
	uniform mat4 projection_matrix;
	layout(location = 0) in vec2 position;
	layout(location = 2) in vec4 color;
	
	smooth out vec4 theColor;
	
	void main() 
	{
		vec4 output_vert;
		output_vert.x = position.x;		
		output_vert.y = position.y;				
		output_vert.z = 0.0f;						
		output_vert.w = 1.0f;
		
		gl_Position = projection_matrix*output_vert;
		theColor = color;
	}
	
	]]
	
	local my_vertex_shader = GLSL_shader(GL.GL_VERTEX_SHADER, vertex_shader_code)
	
	local my_fragment_shader = GLSL_shader(GL.GL_FRAGMENT_SHADER, [[
	#version 330
	smooth in vec4 theColor;
	out vec4 outputColor;
	void main() 
	{
		outputColor = theColor;
	}
	
	]])
	
	local my_shader_program = GLSL_program()
	my_shader_program:attach(my_vertex_shader)
	my_shader_program:attach(my_fragment_shader)
	my_shader_program:use()
	
	local projection_matrix_uniform = GL.glGetUniformLocation(my_shader_program.id, "projection_matrix")
	
	return owner_world:create_entity (override(camera_archetype, {
		transform = {
			pos = vec2(),
			rotation = 0
		},
	
		camera = {
			screen_rect = rect_xywh(0, 0, config_table.resolution_w, config_table.resolution_h),
			size = vec2(config_table.resolution_w, config_table.resolution_h),
			
			drawing_callback = function (subject, camera_draw_input, mask)
				local visible_area = camera_draw_input.visible_area
				
				GL.glUniformMatrix4fv(
				projection_matrix_uniform, 
				1, 
				GL.GL_FALSE, 
				orthographic_projection(0, visible_area.x, visible_area.y, 0, 0, 1):data()
				)
	
				my_shader_program:use()
				
				GL.glClear(GL.GL_COLOR_BUFFER_BIT)
			
				drawing_routine()
			end
		},
		
		input = {
			custom_intents.ZOOM_CAMERA
		},
		
		chase = {},
		
		script_class = camera_class
	}))
end