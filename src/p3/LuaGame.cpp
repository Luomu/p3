#include "p3/LuaGame.h"
#include "p3/p3.h"

namespace p3
{
/*
 * Attribute: time
 *
 * The current game time, in seconds since epoch
 *
 */
static int l_game_attr_time(lua_State* l)
{
	auto sim = p3::game->GetSim();
	if (!sim)
		lua_pushnil(l);
	else
		lua_pushnumber(l, sim->GetGameTime());
	return 1;
}

/*
 * Method: GetTimeAccel
 *
 * Get rate of in-universe time advancement
 *
 */
static int l_game_get_timeaccel(lua_State* l)
{
	auto sim = p3::game->GetSim();
	if (!sim)
		lua_pushnil(l);
	else
		lua_pushnumber(l, sim->GetTimeAccelRate());
	return 1;
}

/*
 * Method: SetTimeAccel
 *
 * Set rate of in-universe time advancement
 *
 */
static int l_game_set_timeaccel(lua_State* l)
{
	auto sim = p3::game->GetSim();
	if (sim) {
		double nr = luaL_checknumber(l, 1);
		sim->SetTimeAccelRate(nr);
	}
	return 0;
}

void LuaGame::Register()
{
	lua_State* l = Lua::manager->GetLuaState();

	LUA_DEBUG_START(l);

	static const luaL_Reg l_methods[] = {
		{ "GetTimeAccel", l_game_get_timeaccel },
		{ "SetTimeAccel", l_game_set_timeaccel },
		{ 0, 0 }
	};

	static const luaL_Reg l_attrs[] = {
		{ "time",      l_game_attr_time      },
		{ 0, 0 }
	};

	lua_getfield(l, LUA_REGISTRYINDEX, "CoreImports");
	LuaObjectBase::CreateObject(l_methods, l_attrs, 0);
	lua_setfield(l, -2, "Game");
	lua_pop(l, 1);

	LUA_DEBUG_END(l, 0);
}
}
