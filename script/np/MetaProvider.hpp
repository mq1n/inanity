#ifndef ___INANITY_SCRIPT_NP_META_PROVIDER_HPP___
#define ___INANITY_SCRIPT_NP_META_PROVIDER_HPP___

#include "np.hpp"
#include "../../platform/npapi.hpp"

BEGIN_INANITY_NP

class State;
class ClassBase;

class MetaProvider
{
public:
	typedef bool (*FunctionRoutine)(const NPVariant* args, int argsCount, NPVariant* result);
	typedef bool (*MethodRoutine)(NPObject* thisObject, const NPVariant* args, int argsCount, NPVariant* result);

	class ConstructorBase
	{
	public:
		virtual FunctionRoutine GetThunk() const = 0;
	};

	class NamedCallableBase
	{
	private:
		const char* name;

	public:
		NamedCallableBase(const char* name);

		const char* GetName() const;
	};

	class FunctionBase : public NamedCallableBase
	{
	public:
		FunctionBase(const char* name);

		virtual FunctionRoutine GetThunk() const = 0;
	};

	class MethodBase : public NamedCallableBase
	{
	public:
		MethodBase(const char* name);

		virtual MethodRoutine GetThunk() const = 0;
	};

	template <typename ClassType, typename... Args>
	class Constructor : public ConstructorBase
	{
	public:
		FunctionRoutine GetThunk() const;
	};

	template <typename FunctionType, FunctionType function>
	class Function : public FunctionBase
	{
	public:
		Function(const char* name);

		FunctionRoutine GetThunk() const;
	};

	template <typename MethodType, MethodType method>
	class Method : public MethodBase
	{
	public:
		Method(const char* name);

		MethodRoutine GetThunk() const;
	};

	typedef Np::ClassBase ClassBase;

	struct Traits
	{
		typedef MetaProvider::ConstructorBase ConstructorBase;
		typedef MetaProvider::FunctionBase FunctionBase;
		typedef MetaProvider::MethodBase MethodBase;
		typedef MetaProvider::ClassBase ClassBase;
	};

	template <typename ClassType>
	class Class;
};

END_INANITY_NP

#endif
