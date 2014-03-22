#ifndef KEY_BINDING
#define KEY_BINDING(name, config_name, ui_name, default_value_1, default_value_2)
#endif
#ifndef AXIS_BINDING
#define AXIS_BINDING(name, config_name, ui_name, default_value)
#endif
#ifndef BINDING_GROUP
#define BINDING_GROUP(ui_name)
#endif
#ifndef BINDING_PAGE
#define BINDING_PAGE(ui_name)
#endif
#ifndef BINDING_PAGE_END
#define BINDING_PAGE_END()
#endif

BINDING_PAGE(CONTROLS)

BINDING_GROUP(Lang::MANUAL_CONTROL_MODE)
KEY_BINDING(thrustForward, "BindThrustForward", Lang::THRUST_FORWARD, SDLK_w, 0)
KEY_BINDING(thrustReverse, "BindThrustReverse", Lang::THRUST_REVERSE, SDLK_s, 0)
KEY_BINDING(thrustLeft, "BindThrustLeft", Lang::THRUST_LEFT, SDLK_a, 0)
KEY_BINDING(thrustRight, "BindThrustRight", Lang::THRUST_RIGHT, SDLK_d, 0)
KEY_BINDING(speedIncrease, "BindSpeedIncrease", Lang::INCREASE_SPEED, SDLK_KP_PLUS, 0)
KEY_BINDING(speedDecrease, "BindSpeedDecrease", Lang::DECREASE_SPEED, SDLK_KP_MINUS, 0)
KEY_BINDING(overrideAutoVelocity, "OverrideAutoVelocity", Lang::OVERRIDE_AUTO_VELOCITY, SDLK_LCTRL, 0)

BINDING_GROUP(Lang::SHIP_ORIENTATION)
KEY_BINDING(pitchUp, "BindPitchUp", Lang::PITCH_UP, SDLK_KP_8, 0)
KEY_BINDING(pitchDown, "BindPitchDown", Lang::PITCH_DOWN, SDLK_KP_2, 0)
KEY_BINDING(yawLeft, "BindYawLeft", Lang::YAW_LEFT, SDLK_KP_4, 0)
KEY_BINDING(yawRight, "BindYawRight", Lang::YAW_RIGHT, SDLK_KP_6, 0)
KEY_BINDING(rollLeft, "BindRollLeft", Lang::ROLL_LEFT, SDLK_KP_7, 0)
KEY_BINDING(rollRight, "BindRollRight", Lang::ROLL_RIGHT, SDLK_KP_9, 0)

BINDING_GROUP(Lang::WEAPONS)
KEY_BINDING(firePrimary, "FirePrimary", Lang::FIRE_PRIMARY, SDLK_SPACE, 0)

BINDING_GROUP(Lang::MISCELLANEOUS)
KEY_BINDING(toggleLuaConsole, "BindToggleLuaConsole", Lang::TOGGLE_LUA_CONSOLE, SDLK_BACKQUOTE, 0)
BINDING_PAGE_END()

#undef KEY_BINDING
#undef AXIS_BINDING
#undef BINDING_GROUP
#undef BINDING_PAGE
#undef BINDING_PAGE_END
