#include <stdlib.h>
#include <stdio.h>
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "game_engine.h"
#include "v4p.h"
#include "v4pi.h"
// V4P->lua bindings
extern int luaopen_v4p(lua_State* L); // V4P binding

// Lua VM State
static lua_State* luaVM;
static int argc;
static char** argv;

// BEGIN Functions exposed to Lua ====================================
static int l_g4pGetFramerate(lua_State* luaVM) {
   lua_Number fr = g4pFramerate;
   lua_pushnumber(luaVM, fr); // return value
   return 1; // 1 return value
}

static int l_g4pSetFramerate(lua_State* luaVM) {
   int fr = (int)luaL_checknumber(luaVM, 1);
   g4pSetFramerate(fr);
   return 0; // no return value
}

static int l_g4pMain(lua_State* luaVM) {
   lua_Number rc = g4pMain(argc, argv);
   lua_pushnumber(luaVM, rc); // return value
   return 1; // 1 return value
}
// END Functions exposed to Lua ======================================

Boolean g4pOnInit() {
    // Call to Lua g4pOnInit
    if (luaL_dostring(luaVM, "return g4pOnInit()"))  {
      fprintf(stderr, "lua script error\n");
      return 1;
	}
	printf("lua_gettop %d\n", lua_gettop(luaVM));
    int rc = lua_toboolean(luaVM, lua_gettop(luaVM));
    lua_pop(luaVM, 1); // Take the returned value out of the stack
    printf("coucou %d\n", rc);
    return rc;
}

Boolean g4pOnIterate() {
    // transmit g4pState to LUA in a dumb way
	static char buffer[500];
    snprintf(buffer, 500, "g4pState.buttons={%d,%d,%d,%d,%d,%d,%d,%d}; g4pState.xpen=%d; g4pState.ypen=%d;",
        g4pState.buttons[0], g4pState.buttons[1], g4pState.buttons[2], g4pState.buttons[3], g4pState.buttons[4],
        g4pState.buttons[5], g4pState.buttons[6], g4pState.buttons[7], g4pState.xpen, g4pState.ypen);
    luaL_dostring(luaVM, buffer);

	// Call to Lua g4pOnIterate
    if (luaL_dostring(luaVM, "return g4pOnIterate()")) {
      fprintf(stderr, "lua script error\n");
      return 1;
	}
    int rc = lua_toboolean(luaVM, lua_gettop(luaVM));
    lua_pop(luaVM, 1); // Take the returned value out of the stack
    return rc;
}

Boolean g4pOnFrame() {
   // Call to Lua g4pOnFrame
    if (luaL_dostring(luaVM, "return g4pOnFrame()"))  {
      fprintf(stderr, "lua script error\n");
      return 1;
	}
    int rc = lua_toboolean(luaVM, lua_gettop(luaVM));
    lua_pop(luaVM, 1); // Take the returned value out of the stack
    return rc;
}

void g4pOnQuit() {
   // Call to Lua g4pOnQuit
    luaL_dostring(luaVM, "g4pOnQuit();");
}

int main(int _argc, char** _argv) {
   argc = _argc;
   argv = _argv;
   
   int i;
   
   // g4plogic.lua file
   char* gameEngineLogicLuaFile = getenv("G4PLOGIC");
   if (!gameEngineLogicLuaFile) gameEngineLogicLuaFile = "./g4plogic.lua";

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
   
   // register 'Game Engine' API
   lua_register(luaVM, "g4pGetFramerate", l_g4pGetFramerate);
   lua_register(luaVM, "g4pSetFramerate", l_g4pSetFramerate);
   lua_register(luaVM, "g4pMain", l_g4pMain);
   luaL_dostring(luaVM, "g4pState={buttons={0,0,0,0,0,0,0,0},xpen=0,ypen=0};");

   // load exe arguments
   lua_newtable(luaVM);
   for (i = 0; i < argc; i++) {
     lua_pushnumber(luaVM, i);
     lua_pushstring(luaVM, argv[i]);
     lua_rawset(luaVM, -3);
   }
   lua_setglobal(luaVM, "g4pArg");

   // go!
   luaL_dofile(luaVM, gameEngineLogicLuaFile);

   // bye
   lua_close(luaVM);
}

