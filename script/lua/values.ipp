#ifndef ___INANITY_SCRIPT_LUA_VALUES_IPP___
#define ___INANITY_SCRIPT_LUA_VALUES_IPP___

#include "values.hpp"
#include "stuff.hpp"
#include "userdata.hpp"
#include "lualib.hpp"
#include "../../Exception.hpp"

BEGIN_INANITY_LUA

template <>
struct Value<bool>
{
	typedef bool ValueType;

	static inline bool Get(lua_State* state, int index)
	{
		return !!lua_toboolean(state, index);
	}

	static inline void Push(lua_State* state, bool value)
	{
		lua_pushboolean(state, (int)value);
	}
};

template <>
struct Value<int>
{
	typedef int ValueType;

	static inline int Get(lua_State* state, int index)
	{
		int isnum;
		lua_Integer res = lua_tointegerx(state, index, &isnum);
		if(!isnum)
			THROW("Expected an integer for argument");
		return (int)res;
	}

	static inline void Push(lua_State* state, int value)
	{
		lua_pushinteger(state, (lua_Integer)value);
	}
};

template <>
struct Value<double>
{
	typedef double ValueType;

	static inline double Get(lua_State* state, int index)
	{
		int isnum;
		lua_Number res = lua_tonumberx(state, index, &isnum);
		if(!isnum)
			THROW("Expected a double for argument");
		return (double)res;
	}

	static inline void Push(lua_State* state, double value)
	{
		lua_pushnumber(state, (lua_Number)value);
	}
};

template <>
struct Value<float>
{
	typedef float ValueType;

	static inline float Get(lua_State* state, int index)
	{
		int isnum;
		lua_Number res = lua_tonumberx(state, index, &isnum);
		if(!isnum)
			THROW("Expected a float for argument");
		return (float)res;
	}

	static inline void Push(lua_State* state, float value)
	{
		lua_pushnumber(state, (lua_Number)value);
	}
};

template <>
struct Value<unsigned int>
{
	typedef unsigned int ValueType;

	static inline unsigned int Get(lua_State* state, int index)
	{
		int isnum;
		lua_Integer res = lua_tointegerx(state, index, &isnum);
		if(!isnum)
			THROW("Expected an integer for argument");
		return (unsigned int)res;
	}

	static inline void Push(lua_State* state, unsigned int value)
	{
		lua_pushinteger(state, (lua_Integer)value);
	}
};

template <>
struct Value<long long>
{
	typedef long long ValueType;

	static inline long long Get(lua_State* state, int index)
	{
		int isnum;
		lua_Number res = lua_tonumberx(state, index, &isnum);
		if(!isnum)
			THROW("Expected an integer for argument");
		return (long long)res;
	}

	static inline void Push(lua_State* state, long long value)
	{
		lua_pushnumber(state, (lua_Number)value);
	}
};

template <>
struct Value<unsigned long long>
{
	typedef unsigned long long ValueType;

	static inline unsigned long long Get(lua_State* state, int index)
	{
		int isnum;
		lua_Number res = lua_tonumberx(state, index, &isnum);
		if(!isnum)
			THROW("Expected an integer for argument");
		return (unsigned long long)res;
	}

	static inline void Push(lua_State* state, unsigned long long value)
	{
		lua_pushnumber(state, (lua_Number)value);
	}
};

template <>
struct Value<const char*>
{
	typedef const char* ValueType;

	static inline const char* Get(lua_State* state, int index)
	{
		const char* res = lua_tostring(state, index);
		if(!res)
			THROW("Expected a string for argument");
		return res;
	}

	static inline void Push(lua_State* state, const char* value)
	{
		lua_pushstring(state, value);
	}
};

template <>
struct Value<String>
{
	typedef String ValueType;

	static inline String Get(lua_State* state, int index)
	{
		return Value<const char*>::Get(state, index);
	}

	static inline void Push(lua_State* state, String value)
	{
		lua_pushstring(state, value.c_str());
	}
};

template <>
struct Value<const String&>
{
	typedef String ValueType;

	static inline String Get(lua_State* state, int index)
	{
		return Value<const char*>::Get(state, index);
	}
};

template <typename ObjectType>
struct Value<ptr<ObjectType> >
{
	typedef ptr<ObjectType> ValueType;

	static inline ptr<ObjectType> Get(lua_State* state, int index)
	{
		// проверить, если это nil (это допустимо)
		if(lua_isnil(state, index))
			return 0;

		// получить userdata для объекта, и проверить, что это объект
		ObjectUserData* userData = (ObjectUserData*)lua_touserdata(state, index);
		if(!userData || lua_islightuserdata(state, index) || userData->type != UserData::typeObject)
			THROW(String("Expected an object of type '") + ObjectType::GetMeta()->GetFullName() + "' for argument, but got " + DescribeValue(state, index));

		// проверить тип объекта, в случае необходимости привести к вышестоящему типу
		for(Meta::ClassBase* cls = userData->cls; cls; cls = cls->GetParent())
			if(cls == ObjectType::GetMeta())
				// вернуть объект
				return (ObjectType*)userData->object;
		// если здесь, значит, мы проверили всю цепочку наследования, а тип не нашли
		THROW(String("Can't cast object of type '") + userData->cls->GetFullName() + "' to expected type '" + ObjectType::GetMeta()->GetFullName() + "'");
	}

	static inline void Push(lua_State* state, ptr<ObjectType> value)
	{
		// проверить, если указатель нулевой, то запустить nil (это допустимо)
		if(!value)
		{
			lua_pushnil(state);
			return;
		}

		ObjectUserData* userData = (ObjectUserData*)lua_newuserdata(state, sizeof(ObjectUserData));
		userData->type = UserData::typeObject;
		userData->object = (RefCounted*)(ObjectType*)value;
		userData->cls = ObjectType::GetMeta();
		// указать метатаблицу
		PushObjectMetaTable(state, ObjectType::GetMeta());
		lua_setmetatable(state, -2);
		// задать дополнительную ссылку объекту
		userData->object->Reference();
	}
};

END_INANITY_LUA

#endif
