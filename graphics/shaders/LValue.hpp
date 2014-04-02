#ifndef ___INANITY_GRAPHICS_SHADERS_LVALUE_HPP___
#define ___INANITY_GRAPHICS_SHADERS_LVALUE_HPP___

#include "Value.hpp"

BEGIN_INANITY_SHADERS

/// Класс L-value значения.
/** То есть такого, которое можно присваивать. */
template <typename ValueType>
class LValue : public Value<ValueType>
{
public:
	LValue(ptr<Node> node);

	/// Оператор присваивания.
	Value<ValueType> operator=(LValue<ValueType> a);
	Value<ValueType> operator=(Value<ValueType> a);

	/// Перестановка компонент (для вектора).
	template <int n>
	LValue<typename SwizzleHelper<ValueType, n>::Type> operator[](const char (&map)[n]);
};

END_INANITY_SHADERS

#endif
