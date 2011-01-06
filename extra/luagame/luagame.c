#include <stdlib.h>
#include <stdio.h>
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "gamemachine.h"
#include "v4p.h"
#include "v4pi.h"
// V4P->lua bindings
extern int luaopen_v4p(lua_State* L); // V4P binding

// Lua VM State
static lua_State* luaVM;
static int argc;
static char** argv;

// BEGIN Functions exposed to Lua ====================================
static int l_gmGetFramerate(lua_State* luaVM) {
   lua_Number fr = gmFramerate;
   lua_pushnumber(luaVM, fr); // return value
   return 1; // 1 return value
}

static int l_gmSetFramerate(lua_State* luaVM) {
   int fr = (int)luaL_checknumber(luaVM, 1);
   gmSetFramerate(fr);
   return 0; // no return value
}

static int l_gmMain(lua_State* luaVM) {
   lua_Number rc = gmMain(argc, argv);
   lua_pushnumber(luaVM, rc); // return value
   return 1; // 1 return value
}
// END Functions exposed to Lua ======================================

Boolean gmOnInit() {
    // Call to Lua gmOnInit
    if (luaL_dostring(luaVM, "return gmOnInit()"))  {
      fprintf(stderr, "lua script error\n");
      return 1;
	}
	printf("lua_gettop %d\n", lua_gettop(luaVM));
    int rc = lua_toboolean(luaVM, lua_gettop(luaVM));
    lua_pop(luaVM, 1); // Take the returned value out of the stack
    printf("coucou %d\n", rc);
    return rc;
}

Boolean gmOnIterate() {
    // transmit gmState to LUA in a dumb way
	static char buffer[500];
    snprintf(buffer, 500, "gmMachineState.buttons={%d,%d,%d,%d,%d,%d,%d,%d}; gmState.xpen=%d; gmState.ypen=%d;",
        gmMachineState.buttons[0], gmMachineState.buttons[1], gmMachineState.buttons[2], gmMachineState.buttons[3], gmMachineState.buttons[4],
        gmMachineState.buttons[5], gmMachineState.buttons[6], gmMachineState.buttons[7], gmMachineState.xpen, gmMachineState.ypen);
    luaL_dostring(luaVM, buffer);

	// Call to Lua gmOnIterate
    if (luaL_dostring(luaVM, "return gmOnIterate()")) {
      fprintf(stderr, "lua script error\n");
      return 1;
	}
    int rc = lua_toboolean(luaVM, lua_gettop(luaVM));
    lua_pop(luaVM, 1); // Take the returned value out of the stack
    return rc;
}

Boolean gmOnFrame() {
   // Call to Lua gmOnFrame
    if (luaL_dostring(luaVM, "return gmOnFrame()"))  {
      fprintf(stderr, "lua script error\n");
      return 1;
	}
    int rc = lua_toboolean(luaVM, lua_gettop(luaVM));
    lua_pop(luaVM, 1); // Take the returned value out of the stack
    return rc;
}

void gmOnQuit() {
   // Call to Lua gmOnQuit
    luaL_dostring(luaVM, "gmOnQuit();");
}

int main(int _argc, char** _argv) {
   argc = _argc;
   argv = _argv;
   
   int i;
   
   // gmlogic.lua file
   char* gameMachineLogicLuaFile = getenv("GMLOGIC");
   if (!gameMachineLogicLuaFile) gameMachineLogicLuaFile = "./gmlogic.lua";

   // Init Lua VM
   luaVM = luaL_newstate();
   if (!luaVM) {
      fprintf(stderr, "Error Initializing lua\n");
      return -1;
   }
 
   // initialize lua standard library functions
   lua_pushcfunction(luaVM, luaopen_base);
   lua_pushstring(luaVM, "");
   lua_call(luaVM, 1, 0);

   lua_pushcfunction(luaVM, luaopen_string);
   lua_pushstring(luaVM, LUA_STRLIBNAME);
   lua_call(luaVM, 1, 0);

   lua_pushcfunction(luaVM, luaopen_io);
   lua_pushstring(luaVM, LUA_IOLIBNAME);
   lua_call(luaVM, 1, 0);

   lua_pushcfunction(luaVM, luaopen_math);
   lua_pushstring(luaVM, LUA_MATHLIBNAME);
   lua_call(luaVM, 1, 0);

   // initialize v4p API
   luaopen_v4p(luaVM);
   
   // register 'GameMachine' API
   lua_register(luaVM, "gmGetFramerate", l_gmGetFramerate);
   lua_register(luaVM, "gmSetFramerate", l_gmSetFramerate);
   lua_register(luaVM, "gmMain", l_gmMain);
   luaL_dostring(luaVM, "gmState={buttons={0,0,0,0,0,0,0,0},xpen=0,ypen=0};");

   // load exe arguments
   lua_newtable(luaVM);
   for (i = 0; i < argc; i++) {
     lua_pushnumber(luaVM, i);
     lua_pushstring(luaVM, argv[i]);
     lua_rawset(luaVM, -3);
   }
   lua_setglobal(luaVM, "gmArg");

   // go!
   luaL_dofile(luaVM, gameMachineLogicLuaFile);

   // bye
   lua_close(luaVM);
}

