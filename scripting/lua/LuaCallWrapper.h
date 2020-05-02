/*
 * LuaCallWrapper.h, part of VCMI engine
 *
 * Authors: listed in file AUTHORS in main folder
 *
 * License: GNU General Public License v2.0 or later
 * Full text of license available in license.txt file, in main folder
 *
 */

#pragma once

#include "api/Registry.h"
#include "LuaStack.h"
#include "LuaFunctor.h"

namespace scripting
{

//TODO: this should be the only one wrapper type
//
template <typename T, typename M, M m>
class LuaMethodWrapper
{

};

template <typename T, typename R, R(T:: * method)()const>
class LuaMethodWrapper <T, R(T:: *)()const, method>
{
public:
	static int invoke(lua_State * L)
	{
		using UDataType = const T *; //TODO: use 'traits'

		static auto KEY = api::TypeRegistry::get()->getKey<UDataType>();
		static auto functor = std::mem_fn(method);

		LuaStack S(L);

		void * raw = luaL_checkudata(L, 1, KEY);

		if(!raw)
			return S.retVoid();

		auto obj = *(static_cast<UDataType *>(raw));

		S.clear();
		S.push(functor(obj));
		return 1;
	}
};

template <typename T, void(T:: * method)()const>
class LuaMethodWrapper <T, void(T:: *)()const, method>
{
public:
	static int invoke(lua_State * L)
	{
		using UDataType = const T *; //TODO: use 'traits'

		static auto KEY = api::TypeRegistry::get()->getKey<UDataType>();
		static auto functor = std::mem_fn(method);

		LuaStack S(L);

		void * raw = luaL_checkudata(L, 1, KEY);

		if(!raw)
			return S.retVoid();

		lua_remove(L, 1);

		auto obj = *(static_cast<UDataType *>(raw));

		functor(obj);

		return S.retVoid();
	}
};

//deprecated, should use LuaMethodWrapper instead, once implemented
template <typename T>
class LuaCallWrapper
{
public:
	using Wrapped = typename std::remove_const<T>::type;

	static std::function<int(lua_State *, T *)> createFunctor(void (Wrapped::* method)())
	{
		auto functor = std::mem_fn(method);
		auto ret = [=](lua_State * L, T * object)->int
		{
			LuaStack S(L);
			functor(object);
			return S.retVoid();
		};

		return ret;
	}

	static std::function<int(lua_State *, T *)> createFunctor(void (Wrapped::* method)() const)
	{
		auto functor = std::mem_fn(method);
		auto ret = [=](lua_State * L, T * object)->int
		{
			LuaStack S(L);
			functor(object);
			return S.retVoid();
		};

		return ret;
	}

	template <typename R>
	static std::function<int(lua_State *, T *)> createFunctor(R (Wrapped::* method)())
	{
		auto functor = std::mem_fn(method);
		auto ret = [=](lua_State * L, T * object)->int
		{
			LuaStack S(L);
			S.push(functor(object));
			return 1;
		};

		return ret;
	}

	template <typename R>
	static std::function<int(lua_State *, T *)> createFunctor(R (Wrapped::* method)() const)
	{
		auto functor = std::mem_fn(method);
		auto ret = [=](lua_State * L, T * object)->int
		{
			LuaStack S(L);
			S.push(functor(object));
			return 1;
		};

		return ret;
	}

	template <typename P1>
	static std::function<int(lua_State *, T *)> createFunctor(void (Wrapped::* method)(P1))
	{
		auto functor = std::mem_fn(method);
		auto ret = [=](lua_State * L, T * object)->int
		{
			LuaStack S(L);
			P1 p1;
			if(S.tryGet(1, p1))
			{
				functor(object, p1);
			}

			return S.retVoid();
		};

		return ret;
	}

	template <typename P1>
	static std::function<int(lua_State *, T *)> createFunctor(void (Wrapped::* method)(P1) const)
	{
		auto functor = std::mem_fn(method);
		auto ret = [=](lua_State * L, T * object)->int
		{
			LuaStack S(L);
			P1 p1;
			if(S.tryGet(1, p1))
			{
				functor(object, p1);
			}

			return S.retVoid();
		};

		return ret;
	}

	template <typename R, typename P1>
	static std::function<int(lua_State *, T *)> createFunctor(R (Wrapped::* method)(P1))
	{
		auto functor = std::mem_fn(method);
		auto ret = [=](lua_State * L, T * object)->int
		{
			LuaStack S(L);
			P1 p1;
			if(S.tryGet(1, p1))
			{
				S.push(functor(object, p1));
				return 1;
			}

			return S.retVoid();
		};

		return ret;
	}

	template <typename R, typename P1>
	static std::function<int(lua_State *, T *)> createFunctor(R (Wrapped::* method)(P1) const)
	{
		auto functor = std::mem_fn(method);
		auto ret = [=](lua_State * L, T * object)->int
		{
			LuaStack S(L);
			P1 p1;
			if(S.tryGet(1, p1))
			{
				S.push(functor(object, p1));
				return 1;
			}

			return S.retVoid();
		};

		return ret;
	}

};



}
