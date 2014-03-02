#include "p3/LuaEngine.h"
#include "p3/p3.h"
#include "p3/Game.h"
#include "pi/LuaObject.h"
#include "p3/GameConfig.h"
#include "ui/Context.h"
#include <stdexcept>

#define UNIMPL() throw std::runtime_error("Unimplemented")
#define CFG() static_cast<p3::GameConfig*>(p3::game->GetConfig())

namespace p3
{

/*
 * Interface: Engine
 *
 * A global table that exposes a number of non-game-specific values from the
 * game engine.
 *
 */

/*
 * Attribute: rand
 *
 * The global <Rand> object. Its stream of values will be different across
 * multiple Pioneer runs. Use this when you just need a random number and
 * don't care about the seed.
 *
 */
static int l_engine_attr_rand(lua_State *l)
{
	UNIMPL();
	//LuaObject<Random>::PushToLua(&Pi::rng);
	return 1;
}

/*
 * Attribute: ticks
 *
 * Number of milliseconds since Pioneer was started. This should be used for
 * debugging purposes only (eg timing) and should never be used for game logic
 * of any kind.
 *
 */
static int l_engine_attr_ticks(lua_State *l)
{
	lua_pushinteger(l, SDL_GetTicks());
	return 1;
}

/*
 * Attribute: ui
 *
 * The global <UI.Context> object. New UI widgets are created through this
 * object.
 *
 */
static int l_engine_attr_ui(lua_State *l)
{
	LuaObject<UI::Context>::PushToLua(p3::game->GetUI());
	return 1;
}

/*
 * Attribute: version
 *
 * String describing the version of Pioneer
 *
 */
static int l_engine_attr_version(lua_State *l)
{
	std::string version(PIONEER_VERSION);
	if (strlen(PIONEER_EXTRAVERSION)) version += " (" PIONEER_EXTRAVERSION ")";
    lua_pushlstring(l, version.c_str(), version.size());
    return 1;
}

/*
 * Function: Quit
 *
 * Exit the program. If there is a game running it ends the game first.
 *
 * > Engine.Quit()
 *
 */
static int l_engine_quit(lua_State *l)
{
	UNIMPL();
	//if (Pi::game)
	//	Pi::EndGame();
	//Pi::Quit();
	return 0;
}

static int l_engine_get_video_mode_list(lua_State *l)
{
	LUA_DEBUG_START(l);

	const std::vector<Graphics::VideoMode> modes = Graphics::GetAvailableVideoModes();
	const int N = modes.size();
	lua_createtable(l, N, 0);
	for (int i = 0; i < N; ++i) {
		lua_createtable(l, 0, 2);
		lua_pushinteger(l, modes[i].width);
		lua_setfield(l, -2, "width");
		lua_pushinteger(l, modes[i].height);
		lua_setfield(l, -2, "height");

		lua_rawseti(l, -2, i+1);
	}

	LUA_DEBUG_END(l, 1);
	return 1;
}

static int l_engine_get_video_resolution(lua_State *l)
{
	UNIMPL(); //should return from renderer
	lua_pushinteger(l, Graphics::GetScreenWidth());
	lua_pushinteger(l, Graphics::GetScreenHeight());
	return 2;
}

static int l_engine_set_video_resolution(lua_State *l)
{
	const int width = luaL_checkinteger(l, 1);
	const int height = luaL_checkinteger(l, 2);
	CFG()->SetInt("ScrWidth", width);
	CFG()->SetInt("ScrHeight", height);
	return 0;
}

static int l_engine_get_fullscreen(lua_State *l)
{
	lua_pushboolean(l, CFG()->Int("StartFullscreen") != 0);
	return 1;
}

static int l_engine_set_fullscreen(lua_State *l)
{
	if (lua_isnone(l, 1))
		return luaL_error(l, "SetFullscreen takes one boolean argument");
	const bool fullscreen = lua_toboolean(l, 1);
	CFG()->SetInt("StartFullscreen", (fullscreen ? 1 : 0));
	return 0;
}

static int l_engine_get_vsync_enabled(lua_State *l)
{
	lua_pushboolean(l, CFG()->Int("VSync") != 0);
	return 1;
}

static int l_engine_set_vsync_enabled(lua_State *l)
{
	if (lua_isnone(l, 1))
		return luaL_error(l, "SetVSyncEnabled takes one boolean argument");
	const bool vsync = lua_toboolean(l, 1);
	CFG()->SetInt("VSync", (vsync ? 1 : 0));
	return 0;
}

static int l_engine_get_texture_compression_enabled(lua_State *l)
{
	lua_pushboolean(l, CFG()->Int("UseTextureCompression") != 0);
	return 1;
}

static int l_engine_set_texture_compression_enabled(lua_State *l)
{
	if (lua_isnone(l, 1))
		return luaL_error(l, "SetTextureCompressionEnabled takes one boolean argument");
	const bool enabled = lua_toboolean(l, 1);
	CFG()->SetInt("UseTextureCompression", (enabled ? 1 : 0));
	return 0;
}

static int l_engine_get_multisampling(lua_State *l)
{
	lua_pushinteger(l, CFG()->Int("AntiAliasingMode"));
	return 1;
}

static int l_engine_set_multisampling(lua_State *l)
{
	const int samples = luaL_checkinteger(l, 1);
	CFG()->SetInt("AntiAliasingMode", samples);
	return 0;
}

#if 0
static void push_bindings(lua_State *l, const KeyBindings::BindingPrototype *protos) {
	LUA_DEBUG_START(l);

	lua_newtable(l); // [-1] bindings
	lua_pushnil(l); // [-2] bindings, [-1] group (no current group)

	assert(!protos[0].function); // first entry should be a group header

	int group_idx = 1;
	int binding_idx = 1;
	for (const KeyBindings::BindingPrototype *proto = protos; proto->label; ++proto) {
		if (! proto->function) {
			// start a new named binding group

			// [-2] bindings, [-1] group
			lua_pop(l, 1);
			// [-1] bindings
			lua_newtable(l);
			lua_pushstring(l, proto->label);
			lua_setfield(l, -2, "label");
			// [-2] bindings, [-1] group
			lua_pushvalue(l, -1);
			// [-3] bindings, [-2] group, [-1] group copy
			lua_rawseti(l, -3, group_idx);
			++group_idx;

			binding_idx = 1;
		} else {
			// key or axis binding prototype

			// [-2] bindings, [-1] group
			lua_createtable(l, 0, 5);
			// [-3] bindings, [-2] group, [-1] binding

			// fields: type ('KEY' or 'AXIS'), id ('BindIncreaseSpeed'), label ('Increase Speed'), binding ('Key13'), bindingDescription ('')
			lua_pushstring(l, (proto->kb ? "KEY" : "AXIS"));
			lua_setfield(l, -2, "type");
			lua_pushstring(l, proto->function);
			lua_setfield(l, -2, "id");
			lua_pushstring(l, proto->label);
			lua_setfield(l, -2, "label");
			if (proto->kb) {
				const KeyBindings::KeyBinding kb1 = proto->kb->binding1;
				if (kb1.Enabled()) {
					lua_pushstring(l, kb1.ToString().c_str());
					lua_setfield(l, -2, "binding1");
					lua_pushstring(l, kb1.Description().c_str());
					lua_setfield(l, -2, "bindingDescription1");
				}
				const KeyBindings::KeyBinding kb2 = proto->kb->binding2;
				if (kb2.Enabled()) {
					lua_pushstring(l, kb2.ToString().c_str());
					lua_setfield(l, -2, "binding2");
					lua_pushstring(l, kb2.Description().c_str());
					lua_setfield(l, -2, "bindingDescription2");
				}
			} else if (proto->ab) {
				const KeyBindings::AxisBinding &ab = *proto->ab;
				lua_pushstring(l, ab.ToString().c_str());
				lua_setfield(l, -2, "binding1");
				lua_pushstring(l, ab.Description().c_str());
				lua_setfield(l, -2, "bindingDescription1");
			} else {
				assert(0); // invalid prototype binding
			}

			// [-3] bindings, [-2] group, [-1] binding
			lua_rawseti(l, -2, binding_idx);
			++binding_idx;
		}

		LUA_DEBUG_CHECK(l, 2); // [-2] bindings, [-1] group
	}

	// pop the group table (which should already have been put in the bindings table)
	lua_pop(l, 1);

	LUA_DEBUG_END(l, 1);
}

/*
 * Function: GetKeyBindings
 *
 * Get a table listing all the current key and axis bindings.
 *
 * > bindings = Engine.GetKeyBindings()
 *
 * Returns:
 *
 *   bindings - A table containing all the key and axis bindings.
 *
 * The bindings table has the following structure (in Lua syntax):
 *
 * bindings = {
 *   { -- a page
 *      label = 'CONTROLS', -- the (translated) name of the page
 *      { -- a group
 *          label = 'Miscellaneous', -- the (translated) name of the group
 *          { -- a binding
 *              type = 'KEY', -- the type of binding; can be 'KEY' or 'AXIS'
 *              id = 'BindToggleLuaConsole', -- the internal ID of the binding; pass this to Engine.SetKeyBinding
 *              label = 'Toggle Lua console', -- the (translated) label for the binding
 *              binding1 = 'Key96', -- the first bound key or axis (value stored in config file)
 *              bindingDescription1 = '`', -- display text for the first bound key or axis
 *              binding2 = 'Key96', -- the second bound key or axis (value stored in config file)
 *              bindingDescription2 = '`', -- display text for the second bound key or axis
 *          },
 *          -- ... more bindings
 *      },
 *      -- ... more groups
 *   },
 *   -- ... more pages
 * }
 *
 * Availability:
 *
 *   October 2013
 *
 * Status:
 *
 *   temporary
 */
static int l_engine_get_key_bindings(lua_State *l)
{
	// XXX maybe this key-bindings table should be cached in the Lua registry?

	int idx = 1;
	lua_newtable(l);

#define BINDING_PAGE(name) \
	push_bindings(l, KeyBindings :: BINDING_PROTOS_ ## name); \
	lua_pushstring(l, Lang :: name); \
	lua_setfield(l, -2, "label"); \
	lua_rawseti(l, -2, idx++);
#include "KeyBindings.inc.h"

	return 1;
}

static int set_key_binding(lua_State *l, const char *config_id, KeyBindings::KeyAction *action) {
	const char *binding_config_1 = lua_tostring(l, 2);
	const char *binding_config_2 = lua_tostring(l, 3);
	KeyBindings::KeyBinding kb1, kb2;
	if (binding_config_1) {
		if (!KeyBindings::KeyBinding::FromString(binding_config_1, kb1))
			return luaL_error(l, "invalid first key binding given to Engine.SetKeyBinding");
	} else
		kb1.Clear();
	if (binding_config_2) {
		if (!KeyBindings::KeyBinding::FromString(binding_config_2, kb2))
			return luaL_error(l, "invalid second key binding given to Engine.SetKeyBinding");
	} else
		kb2.Clear();
	action->binding1 = kb1;
	action->binding2 = kb2;
	Pi::config->SetString(config_id, action->ToString());
	Pi::config->Save();
	return 0;
}

static int set_axis_binding(lua_State *l, const char *config_id, KeyBindings::AxisBinding *binding) {
	const char *binding_config = luaL_checkstring(l, 2);
	KeyBindings::AxisBinding ab;
	if (!KeyBindings::AxisBinding::FromString(binding_config, ab))
		return luaL_error(l, "invalid axis binding given to Engine.SetKeyBinding");
	Pi::config->SetString(config_id, ab.ToString());
	Pi::config->Save();
	*binding = ab;
	return 0;
}

static int l_engine_set_key_binding(lua_State *l)
{
	const char *binding_id = luaL_checkstring(l, 1);

#define KEY_BINDING(action, config_id, label, def1, def2) \
	if (strcmp(binding_id, config_id) == 0) { return set_key_binding(l, config_id, &KeyBindings :: action); }
#define AXIS_BINDING(action, config_id, label, default_axis) \
	if (strcmp(binding_id, config_id) == 0) { return set_axis_binding(l, config_id, &KeyBindings :: action); }

#include "KeyBindings.inc.h"

	return luaL_error(l, "Invalid binding ID given to Engine.SetKeyBinding");
}

static int l_engine_get_mouse_y_inverted(lua_State *l)
{
	lua_pushboolean(l, Pi::config->Int("InvertMouseY") != 0);
	return 1;
}

static int l_engine_set_mouse_y_inverted(lua_State *l)
{
	if (lua_isnone(l, 1))
		return luaL_error(l, "SetMouseYInverted takes one boolean argument");
	const bool inverted = lua_toboolean(l, 1);
	Pi::config->SetInt("InvertMouseY", (inverted ? 1 : 0));
	Pi::config->Save();
	Pi::SetMouseYInvert(inverted);
	return 0;
}

static int l_engine_get_joystick_enabled(lua_State *l)
{
	lua_pushboolean(l, Pi::config->Int("EnableJoystick") != 0);
	return 1;
}

static int l_engine_set_joystick_enabled(lua_State *l)
{
	if (lua_isnone(l, 1))
		return luaL_error(l, "SetJoystickEnabled takes one boolean argument");
	const bool enabled = lua_toboolean(l, 1);
	Pi::config->SetInt("EnableJoystick", (enabled ? 1 : 0));
	Pi::config->Save();
	Pi::SetJoystickEnabled(enabled);
	return 0;
}

#endif

void LuaEngine::Register()
{
	lua_State *l = Lua::manager->GetLuaState();

	LUA_DEBUG_START(l);

	static const luaL_Reg l_methods[] = {
		{ "Quit", l_engine_quit },

		{ "GetVideoModeList", l_engine_get_video_mode_list },
		{ "GetVideoResolution", l_engine_get_video_resolution },
		{ "SetVideoResolution", l_engine_set_video_resolution },
		{ "GetFullscreen", l_engine_get_fullscreen },
		{ "SetFullscreen", l_engine_set_fullscreen },
		{ "GetVSyncEnabled", l_engine_get_vsync_enabled },
		{ "SetVSyncEnabled", l_engine_set_vsync_enabled },
		{ "GetTextureCompressionEnabled", l_engine_get_texture_compression_enabled },
		{ "SetTextureCompressionEnabled", l_engine_set_texture_compression_enabled },
		{ "GetMultisampling", l_engine_get_multisampling },
		{ "SetMultisampling", l_engine_set_multisampling },

		//{ "GetKeyBindings", l_engine_get_key_bindings },
		//{ "SetKeyBinding", l_engine_set_key_binding },
		//{ "GetMouseYInverted", l_engine_get_mouse_y_inverted },
		//{ "SetMouseYInverted", l_engine_set_mouse_y_inverted },
		//{ "GetJoystickEnabled", l_engine_get_joystick_enabled },
		//{ "SetJoystickEnabled", l_engine_set_joystick_enabled },

		{ 0, 0 }
	};

	static const luaL_Reg l_attrs[] = {
		{ "rand",    l_engine_attr_rand    },
		{ "ticks",   l_engine_attr_ticks   },
		{ "ui",      l_engine_attr_ui      },
		{ "version", l_engine_attr_version },
		{ 0, 0 }
	};

	lua_getfield(l, LUA_REGISTRYINDEX, "CoreImports");
	LuaObjectBase::CreateObject(l_methods, l_attrs, 0);
	lua_setfield(l, -2, "Engine");
	lua_pop(l, 1);

	LUA_DEBUG_END(l, 0);
}

}
