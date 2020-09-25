#include "skynet.h"
#include "skynet_env.h"
#include "spinlock.h"

#include <lua.h>
#include <lauxlib.h>

#include <stdlib.h>
#include <assert.h>

struct skynet_env {
	struct spinlock lock;
	lua_State *L;
};

static struct skynet_env *E = NULL;

const char * 
skynet_getenv(const char *key) {
	SPIN_LOCK(E)

	lua_State *L = E->L;
	
	lua_getglobal(L, key);
	const char * result = lua_tostring(L, -1);
	lua_pop(L, 1);

	SPIN_UNLOCK(E)

	return result;
}

void 
skynet_setenv(const char *key, const char *value) {
	SPIN_LOCK(E)
	
	lua_State *L = E->L;
	lua_getglobal(L, key);
	assert(lua_isnil(L, -1));
	lua_pop(L,1);
	lua_pushstring(L,value);
	lua_setglobal(L,key);

	SPIN_UNLOCK(E)
}

void
skynet_env_init() {
	E = skynet_malloc(sizeof(*E));
	SPIN_INIT(E)
    //lua_newstate主要功能分配并初始化内存空间，准备lua_State\global_State【两个主要的关键的结构体】其中包括垃圾回收机制的初始化及建立、stack初始化、registry初始化、string表、metamethod、语法分析初始化
    //创建一个新的、独立的Lua状态机，如果因为内存不足导致创建失败，返回NULL
	E->L = luaL_newstate();
}
