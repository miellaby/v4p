#include <stdlib.h>
#include <stdio.h>
#include "lua.h"
#include "lauxlib.h"
#include "gamemachine.h"
#include "v4p.h"
#include "v4pi.h"

// Lua VM State
static lua_State* luaVM;

// Functions exposed to Lua
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

Boolean gmOnInit() {
    // Call to Lua gmOnInit
    lua_dostring(luaVM, "gmOnInit();");
}

Boolean gmOnIterate() {
    // transmit gmState to LUA in a dumb way
	static char buffer[500];
    snprintf(buffer, 500, "gmMachineState.buttons={%d,%d,%d,%d,%d,%d,%d,%d}; gmState.xpen=%d; gmState.ypen=%d;",
        gmMachineState.buttons[0], gmMachineState.buttons[1], gmMachineState.buttons[2], gmMachineState.buttons[3], gmMachineState.buttons[4],
        gmMachineState.buttons[5], gmMachineState.buttons[6], gmMachineState.buttons[7], gmMachineState.xpen, gmMachineState.ypen);
    lua_dostring(luaVM, buffer);

	// Call to Lua gmOnIterate
    lua_dostring(luaVM, "gmOnIterate();");
}

Boolean gmOnFrame() {
   // Call to Lua gmOnFrame
    lua_dostring(luaVM, "gmOnFrame();");
}

void gmOnQuit() {
   // Call to Lua gmOnQuit
    lua_dostring(luaVM, "gmOnQuit();");
}

int main(int argc, char** argv) {
   // gmlogic.lua file
   char* gameMachineLogicLuaFile = getenv("GMLOGIC");
   if (!gameMachineLogicLuaFile) gameMachineLogicLuaFile = "./gmlogic.lua";

   // Init Lua VM
   luaVM = lua_open();
   if (!luaVM) {
      fprintf(stderr, "Error Initializing lua\n");
      return -1;
   }
 
   // initialize lua standard library functions
   lua_baselibopen(luaVM);
   lua_iolibopen(luaVM);
   lua_strlibopen(luaVM);
   lua_mathlibopen(luaVM);
   lua_register(luaVM, "gmGetFramerate", l_gmGetFramerate);
   lua_register(luaVM, "gmSetFramerate", l_gmSetFramerate);
   lua_dostring(luaVM, "gmState={buttons={0,0,0,0,0,0,0,0},xpen=0,ypen=0};");
   lua_dofile(luaVM, gameMachineLogicLuaFile);
 
   // call gmMain
   int rc = gmMain(argc, argv);

   // bye
   lua_close(luaVM);
   return rc;
}

