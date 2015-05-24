custom_intents = create_inverse_enum {
	"INCREASE_STEP",
	"DECREASE_STEP",
	"QUIT",
	"SWITCH",
	"ZOOM_CAMERA",
	"REPOPULATE"
}

main_input_context = create_input_context {
	intents = { 
		[mouse.raw_motion] 		= intent_message.AIM,
		[keys.Z] 				= custom_intents.DECREASE_STEP,
		[keys.X] 				= custom_intents.INCREASE_STEP,
		[keys.W] 				= intent_message.MOVE_FORWARD,
		[keys.S] 				= intent_message.MOVE_BACKWARD,
		[keys.A] 				= intent_message.MOVE_LEFT,
		[keys.D] 				= intent_message.MOVE_RIGHT,
		[keys.C] 				= custom_intents.SWITCH,
		[keys.R] 				= custom_intents.REPOPULATE,
		
		[keys.ESC] 				= custom_intents.QUIT,
		[mouse.wheel]			= custom_intents.ZOOM_CAMERA
	}
}