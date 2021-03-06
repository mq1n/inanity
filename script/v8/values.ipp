#ifndef ___INANITY_SCRIPT_V8_VALUES_IPP___
#define ___INANITY_SCRIPT_V8_VALUES_IPP___

#include "values.hpp"
#include "State.hpp"
#include "Any.hpp"
#include "../../String.hpp"
#include "../../Exception.hpp"

BEGIN_INANITY_V8

template <>
struct Value<bool>
{
	typedef bool ValueType;

	static inline bool From(v8::Local<v8::Value> value)
	{
		return value->BooleanValue();
	}

	static inline v8::Local<v8::Value> To(bool value)
	{
		return v8::Boolean::New(State::GetCurrent()->GetIsolate(), value);
	}
};

template <>
struct Value<int>
{
	typedef int ValueType;

	static inline int From(v8::Local<v8::Value> value)
	{
		return (int)value->IntegerValue();
	}

	static inline v8::Local<v8::Value> To(int value)
	{
		return v8::Number::New(State::GetCurrent()->GetIsolate(), (double)value);
	}
};

template <>
struct Value<long>
{
	typedef long ValueType;

	static inline long From(v8::Local<v8::Value> value)
	{
		return (long)value->IntegerValue();
	}

	static inline v8::Local<v8::Value> To(long value)
	{
		return v8::Number::New(State::GetCurrent()->GetIsolate(), (double)value);
	}
};

template <>
struct Value<double>
{
	typedef double ValueType;

	static inline double From(v8::Local<v8::Value> value)
	{
		return value->NumberValue();
	}

	static inline v8::Local<v8::Value> To(double value)
	{
		return v8::Number::New(State::GetCurrent()->GetIsolate(), value);
	}
};

template <>
struct Value<float>
{
	typedef float ValueType;

	static inline float From(v8::Local<v8::Value> value)
	{
		return (float)value->NumberValue();
	}

	static inline v8::Local<v8::Value> To(float value)
	{
		return v8::Number::New(State::GetCurrent()->GetIsolate(), (double)value);
	}
};

template <>
struct Value<unsigned int>
{
	typedef unsigned int ValueType;

	static inline unsigned int From(v8::Local<v8::Value> value)
	{
		return (unsigned int)value->IntegerValue();
	}

	static inline v8::Local<v8::Value> To(unsigned int value)
	{
		return v8::Number::New(State::GetCurrent()->GetIsolate(), (double)value);
	}
};

template <>
struct Value<unsigned long>
{
	typedef unsigned long ValueType;

	static inline unsigned long From(v8::Local<v8::Value> value)
	{
		return (unsigned long)value->IntegerValue();
	}

	static inline v8::Local<v8::Value> To(unsigned long value)
	{
		return v8::Number::New(State::GetCurrent()->GetIsolate(), (double)value);
	}
};

template <>
struct Value<long long>
{
	typedef long long ValueType;

	static inline long long From(v8::Local<v8::Value> value)
	{
		return value->IntegerValue();
	}

	static inline v8::Local<v8::Value> To(long long value)
	{
		return v8::Number::New(State::GetCurrent()->GetIsolate(), (double)value);
	}
};

template <>
struct Value<unsigned long long>
{
	typedef unsigned long long ValueType;

	static inline unsigned long long From(v8::Local<v8::Value> value)
	{
		return value->IntegerValue();
	}

	static inline v8::Local<v8::Value> To(unsigned long long value)
	{
		return v8::Number::New(State::GetCurrent()->GetIsolate(), (double)value);
	}
};

template <>
struct Value<const char*>
{
	typedef String ValueType;

	static inline String From(v8::Local<v8::Value> value)
	{
		v8::String::Utf8Value s(value);
		return String(*s, s.length());
	}

	static inline v8::Local<v8::Value> To(const char* value)
	{
		return v8::String::NewFromUtf8(State::GetCurrent()->GetIsolate(), value);
	}
};

template <>
struct Value<String>
{
	typedef String ValueType;

	static inline String From(v8::Local<v8::Value> value)
	{
		v8::String::Utf8Value s(value);
		return String(*s, s.length());
	}

	static inline v8::Local<v8::Value> To(const String& value)
	{
		return v8::String::NewFromUtf8(
			State::GetCurrent()->GetIsolate(),
			value.c_str(),
			v8::String::kNormalString,
			(int)value.length());
	}
};

template <>
struct Value<const String&>
{
	typedef String ValueType;

	static inline String From(v8::Local<v8::Value> value)
	{
		v8::String::Utf8Value s(value);
		return String(*s, s.length());
	}
};

template <>
struct Value<ptr<Script::Any> >
{
	typedef ptr<Script::Any> ValueType;

	static inline ptr<Script::Any> From(v8::Local<v8::Value> value)
	{
		return State::GetCurrent()->CreateAny(value);
	}

	static inline v8::Local<v8::Value> To(ptr<Script::Any> value)
	{
		return fast_cast<Any*>(&*value)->GetV8Value();
	}
};

/*
ptr<RefCounted> is represented as an External, or as null (zero pointer).
External of corrent object contains non-null pointer.
External with null pointer is a wiped object, i.e. object which was
reclamed from script by C++ code.
Other values are not accepted (i.e. zero, undefined).
*/
template <typename ObjectType>
struct Value<ptr<ObjectType> >
{
	typedef ptr<ObjectType> ValueType;

	static inline ptr<ObjectType> From(v8::Local<v8::Value> value)
	{
		// if it's null object
		if(value->IsNull())
			return 0;

		// get internal field of an object
		v8::Local<v8::Value> thisValue = value->ToObject()->GetInternalField(0);

		// otherwise it should be an external
		if(!thisValue->IsExternal())
		{
			v8::String::Utf8Value s(thisValue);
			const char* classFullName = Meta::MetaOf<MetaProvider, ObjectType>()->GetFullName();
			THROW(classFullName + String(" instance can't be obtained from ") + *s);
		}

		// get the value
		void* externalValue = v8::External::Cast(*thisValue)->Value();

		// if the value is null, the object was reclaimed
		if(!externalValue)
		{
			const char* classFullName = Meta::MetaOf<MetaProvider, ObjectType>()->GetFullName();
			THROW(classFullName + String(" instance was reclaimed"));
		}

		ObjectType* object = fast_cast<ObjectType*>((RefCounted*)externalValue);

		return object;
	}

	static inline v8::Local<v8::Value> To(ptr<ObjectType> value)
	{
		if(value)
			return State::GetCurrent()->ConvertObject(Meta::MetaOf<MetaProvider, ObjectType>(), static_cast<RefCounted*>(&*value));
		else
			return v8::Null(State::GetCurrent()->GetIsolate());
	}
};

template <typename T>
struct Value
{
	typedef T ValueType;

	static inline T From(v8::Local<v8::Value> value)
	{
		return ConvertFromScript<MetaProvider, T>(State::GetCurrent()->CreateAny(value));
	}

	static inline v8::Local<v8::Value> To(const T& value)
	{
		return fast_cast<Any*>(&*ConvertToScript<MetaProvider, T>(State::GetCurrent(), value))->GetV8Value();
	}
};

template <typename T>
struct Value<const T&>
{
	typedef T ValueType;

	static inline T From(v8::Local<v8::Value> value)
	{
		return ConvertFromScript<MetaProvider, T>(State::GetCurrent()->CreateAny(value));
	}

	static inline v8::Local<v8::Value> To(const T& value)
	{
		return fast_cast<Any*>(&*ConvertToScript<MetaProvider, T>(State::GetCurrent(), value))->GetV8Value();
	}
};

END_INANITY_V8

#endif
