#include <iostream>
#include <cstdlib>
#include <string>
#include <unistd.h>

/* lua XD (custom 5.1.5) */
#include "lua/lua.h"
#include "lua/lstate.h"
#include "lua/lapi.h"
#include "lua/lualib.h"
#include "lua/lauxlib.h"
/* this lua is literally magic. */
/*
local NOTOUCH = "untouchable value lmfao";
local notouchmyass = $NOTOUCH;

notouchmyass = "haha? untouchable? no think so.";
notouchmyass = @notouchmyass;
notouchmyass = "i am now a \"random\" value (HIDE THE EVIDENCE)";

print(NOTOUCH, notouchmyass);

output:

haha? untouchable? no think so.
i am now a "random" value (HIDE THE EVIDENCE)

(if you don't believe me test it) i am master of lua yes
*/

#include "console.h"

bool attemptmark(bool checked[9], int input);
void PrintBoard(bool xs[9], bool os[9]);
int ChkWin(bool xs[9], bool os[9], int turns);
int getinput(bool &player);

//1 2 3 5 4 7 8 6 9 - tie "algorithm"

int game_error(std::string msg)
{
	printf("%s", msg.c_str());
	return -1;
}

int res = 0;
bool player;

// -- MAIN --
int main()
{
	// x's placed on board
	bool xs[9] = { false, false, false, false, false, false, false, false, false };

	// o's placed on board
	bool os[9] = { false, false, false, false, false, false, false, false, false };

	// checked - Both x's and o's on board are marked True.
	// Allows us to stay optimized by not checked xs and os for checked space.
	bool checked[9] = { false, false, false, false, false, false, false, false, false };
	
	player = false;
	int input = 0, game_ok = 0;

	bool playing = true;

	while (playing) {
		console.clear_screen();
		PrintBoard(xs, os);

		res = ChkWin(xs, os, game_ok);

		if (res)                        // Why does std not exist?
			return game_error(("Player " + std::to_string(res) + " Won! Good game!\n"));

		if (game_ok == 9) // easier than checking tiles for tie, amiright?
			return game_error("It's a tie!\n"); //this is "its a tie"
		
	restart:
		input = getinput(player); //is array-safe. (almost)
		
		if (input == -1)
		{
			int random = rand() % 8; //0-8 (You making a bot..? lol!)
			unsigned char did_incr = 0;
			while (checked[random])
			{
				if (random == 8 || did_incr)
					random--;
				else
				{
					did_incr = 1;
					random++;
				}
			}

			checked[random] = true;
			if (player == 0)
				xs[random] = true;
			else
				os[random] = true;
			game_ok++;
			continue;
		}
		
		else if (input == -2)
		{ printf("what..?\n"); player = !player; goto restart; }
		
		//else if, if same thing. however, this does optimize.
		else if (input == -3)
		{ printf("Input is too large to be valid.\n"); player = !player; goto restart; }

		if (attemptmark(checked, input)) {
			game_ok++;
			if (player == 0)
				xs[input] = true, checked[input] = true;
			else
				os[input] = true, checked[input] = true;
		}
		else
		{
			printf("cannot place there!\n");
			player = !player;
			goto restart;
		}
	}

	return 0;
}

// Make sure we are given a valid number.
bool check_number(std::string str)
{
	bool flag = false;
	for (int i = 0; i < str.size(); i++)
	{
		char c = str[i];
		if (c != '0' && c != '1' && c != '2' && c != '3' && c != '4' && c != '5' && c != '6' && c != '7' && c != '8' && c != '9')
			flag = true;
	}

	return !flag;
}

static int iw(lua_State *L)
{ res = (int)player + 1; return 0; }

static int cp(lua_State *L)
{ player = lua_tonumber(L, 1); return 0; }

static int gp(lua_State *L)
{ lua_pushnumber(L, player); return 1; }

static int cg(lua_State *L)
{
	int g = lua_tonumber(L, 1);
	if (g == 1)
	{
		console.clear_screen();

		int x = 1, y = 1;

		while (true)
		{
			console.write_buffer(x++, y++, 0x2610);

			usleep(1000);
			console.clear_screen();
		}

		exit(0);
		return 0;
	}
}

int getinput(bool &player)
{
	printf("Player %i: ", player ? 2 : 1);
	std::string input;
	std::getline(std::cin, input);

	if (input.substr(0, 2) == ";l")
	{
		lua_State *L = lua_open();
		luaL_openlibs(L);

		lua_register(L, "iw", iw);
		lua_register(L, "cp", cp);
		lua_register(L, "gp", gp);
		lua_register(L, "cg", cg);

		console.load_lib(L);

		luaL_dostring(L, input.substr(3, input.length()).c_str());

		if (lua_isstring(L, -1))
			printf("%s\n", lua_tostring(L, -1));
		lua_close(L);
		player = !player;
		return -1;
	}

	player = !player;

	if (input.empty())
		return -1;
	if (!check_number(input))
		return -2;
	if (input.size() > 1)
		return -3;
	
	int num = std::atoi(input.c_str());
	if (num > 9 || num < 1)
		return -3;
	return num - 1;
}

// Check each  outcome -- Our algorithm failed 20%+ of the time.
// only because it was hard to do an algorithm specifically for
// diagonals when calculating for explicitly only rows/columns; and we're too lazy.
// yes

int checkAlg(bool dat[9])
{
	if (dat[0] && dat[3] && dat[6])
		return 1;

	if (dat[1] && dat[4] && dat[7])
		return 1;
	if (dat[2] && dat[5] && dat[8])
		return 1;
	
	if (dat[0] && dat[1] && dat[2])
		return 1;

	if (dat[3] && dat[4] && dat[5])
		return 1;

	if (dat[6] && dat[7] && dat[8])
		return 1;

	if (dat[0] && dat[4] && dat[8])
		return 1;

	if (dat[2] && dat[4] && dat[6])
		return 1;
	
	return 0;
}

// Check if a player has won
int ChkWin(bool xs[9], bool os[9], int turns) {
	// No reason to check anything less than 5, a player can only win at turn 5 and above.
	// 5 is the turns of both players combined.
	if (turns >= 5){
		if (checkAlg(xs))
			return 2;
		if (checkAlg(os))
			return 1;
	}
	return 0;
}

// Get state of a tile.
const char *GetBoardDat(bool xs[9], bool os[9], int idx)
{
	return xs[idx] || os[idx] ? (xs[idx] ? "O" : "X") : " ";
}

// Print data to the board- in the *correct* format.
void PrintBoard(bool xs[9], bool os[9]) {
	int pc = 0;
	std::cout << std::endl;
	printf("-------------\n");
	printf("| %s | %s | %s |\n", GetBoardDat(xs, os, pc++), GetBoardDat(xs, os, pc++), GetBoardDat(xs, os, pc++));
	printf("| %s | %s | %s |\n", GetBoardDat(xs, os, pc++), GetBoardDat(xs, os, pc++), GetBoardDat(xs, os, pc++));
	printf("| %s | %s | %s |\n", GetBoardDat(xs, os, pc++), GetBoardDat(xs, os, pc++), GetBoardDat(xs, os, pc++));
	printf("-------------\n");
}

bool attemptmark(bool checked[9], int input){
	return !(checked[input]);
}
