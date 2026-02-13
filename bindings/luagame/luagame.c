#include <stdlib.h>
#include <stdio.h>
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
#include "g4p.h"
#include "v4p.h"
#include "v4pi.h"
// V4P->lua bindings
extern int luaopen_v4p(lua_State* L);  // V4P binding

// Lua VM State
static lua_State* luaVM;
static int argc;
static char** argv;

// BEGIN Functions exposed to Lua ====================================
static int l_g4pGetFramerate(lua_State* luaVM) {
    lua_Number fr = g4p_framerate;
    lua_pushnumber(luaVM, fr);  // return value
    return 1;  // 1 return value
}

static int l_g4pSetFramerate(lua_State* luaVM) {
    int fr = (int) luaL_checknumber(luaVM, 1);
    g4p_setFramerate(fr);
    return 0;  // no return value
}

static int l_g4pMain(lua_State* luaVM) {
    lua_Number rc = g4p_main(argc, argv);
    lua_pushnumber(luaVM, rc);  // return value
    return 1;  // 1 return value
}
// END Functions exposed to Lua ======================================

Boolean g4p_onInit(int quality, Boolean fullscreen) {
    // Call to Lua g4pOnInit
    if (luaL_dostring(luaVM, "return g4pOnInit()")) {
        fprintf(stderr, "lua script error\n");
        return 1;
    }
    printf("lua_gettop %d\n", lua_gettop(luaVM));
    int rc = lua_toboolean(luaVM, lua_gettop(luaVM));
    lua_pop(luaVM, 1);  // Take the returned value out of the stack
    printf("coucou %d\n", rc);
    return rc;
}

Boolean g4p_onTick(Int32 deltaTime) {
    // transmit g4p_state to LUA in a dumb way
    static char buffer[500];
    snprintf(buffer,
             500,
             "g4p_state.buttons={%d,%d,%d,%d,%d,%d,%d,%d,%d};"
             " g4p_state.xpen=%d; g4p_state.ypen=%d; g4p_state.deltaTime=%d;",
             g4p_state.buttons[0],
             g4p_state.buttons[1],
             g4p_state.buttons[2],
             g4p_state.buttons[3],
             g4p_state.buttons[4],
             g4p_state.buttons[5],
             g4p_state.buttons[6],
             g4p_state.buttons[7],
             g4p_state.buttons[8],
             g4p_state.xpen,
             g4p_state.ypen,
             deltaTime);
    luaL_dostring(luaVM, buffer);

    // Call to Lua g4pOnTick
    if (luaL_dostring(luaVM, "return g4pOnTick(g4p_state.deltaTime)")) {
        fprintf(stderr, "lua script error\n");
        return 1;
    }
    int rc = lua_toboolean(luaVM, lua_gettop(luaVM));
    lua_pop(luaVM, 1);  // Take the returned value out of the stack
    return rc;
}

Boolean g4p_onFrame() {
    // Call to Lua g4pOnFrame
    if (luaL_dostring(luaVM, "return g4pOnFrame()")) {
        fprintf(stderr, "lua script error\n");
        return 1;
    }
    int rc = lua_toboolean(luaVM, lua_gettop(luaVM));
    lua_pop(luaVM, 1);  // Take the returned value out of the stack
    return rc;
}

void g4p_onQuit() {
    // Call to Lua g4pOnQuit
    luaL_dostring(luaVM, "g4pOnQuit();");
}

int main(int _argc, char** _argv) {
    argc = _argc;
    argv = _argv;

    int i;

    // g4plogic.lua file
    char* gameEngineLogicLuaFile = getenv("G4PLOGIC");
    if (! gameEngineLogicLuaFile)
        gameEngineLogicLuaFile = "./g4plogic.lua";

    // Init Lua VM
    luaVM = luaL_newstate();
    if (! luaVM) {
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
    lua_register(luaVM, "g4p_setFramerate", l_g4pSetFramerate);
    lua_register(luaVM, "g4pMain", l_g4pMain);
    luaL_dostring(luaVM, "g4p_state={buttons={0,0,0,0,0,0,0,0},xpen=0,ypen=0,deltaTime=0};");

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
