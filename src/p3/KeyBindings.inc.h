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
KEY_BINDING(thrustIncrease, "BindThrustIncrease", Lang::INCREASE_THRUST, SDLK_a, 0)
KEY_BINDING(thrustDecrease, "BindThrustDecrease", Lang::DECREASE_THRUST, SDLK_z, 0)

BINDING_GROUP(Lang::MISCELLANEOUS)
KEY_BINDING(toggleLuaConsole, "BindToggleLuaConsole", Lang::TOGGLE_LUA_CONSOLE, SDLK_BACKQUOTE, 0)
BINDING_PAGE_END()

#undef KEY_BINDING
#undef AXIS_BINDING
#undef BINDING_GROUP
#undef BINDING_PAGE
#undef BINDING_PAGE_END
