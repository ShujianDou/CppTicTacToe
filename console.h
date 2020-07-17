#ifndef CONSOLE_H
#define CONSOLE_H
#include <cstdlib>
#include <iostream>

#include "lua/lua.h"
#include "lua/lstate.h"
#include "lua/lapi.h"
#include "lua/lualib.h"
#include "lua/lauxlib.h"

static int lua_clear_screen(lua_State *L)
{
	printf("\033[2J");
}

static int lua_write_buffer(lua_State *L)
{
	printf("\033[%i;%iH", lua_tonumber(L, 1), lua_tonumber(L, 2));
	printf("%s", lua_tostring(L, 3));
}

class CONSOLE {
private:
public:
	void clear_screen()
	{
		printf("\033[2J");
	}

	void write_buffer(int x, int y, short b)
	{
		printf("\033[%i;%iH", x, y);
		printf("%c", b);
	}

	void write_buffer(int x, int y, const char *b)
	{
		printf("\033[%i;%iH", x, y);
		printf("%s", b);
	}

	void load_lib(lua_State *L)
	{
		lua_newtable(L);
		{
			lua_pushcfunction(L, lua_clear_screen);
			lua_setfield(L, -2, "clear");

			lua_pushcfunction(L, lua_write_buffer);
			lua_setfield(L, -2, "write");
		}
		lua_setglobal(L, "console");
	}
};

CONSOLE console; //lul.
#endif
