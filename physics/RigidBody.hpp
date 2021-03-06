#ifndef ___INANITY_PHYSICS_RIGID_BODY_HPP___
#define ___INANITY_PHYSICS_RIGID_BODY_HPP___

#include "physics.hpp"
#include "../meta/decl.hpp"

BEGIN_INANITY_PHYSICS

class World;
class Shape;

/// Абстрактный класс твёрдого тела.
class RigidBody : public Object
{
protected:
	/// Физический мир, в который добавлено тело.
	ptr<World> world;
	/// Форма твёрдого тела.
	ptr<Shape> shape;

protected:
	RigidBody(ptr<World> world, ptr<Shape> shape);

public:
	ptr<World> GetWorld() const;
	ptr<Shape> GetShape() const;

	/// Получить текущее положение твёрдого тела.
	virtual vec3 GetPosition() const = 0;
	/// Получить текущую ориентацию твёрдого тела.
	virtual mat3x3 GetOrientation() const = 0;
	/// Получить текущую трансформацию твёрдого тела.
	virtual mat4x4 GetTransform() const = 0;

	/// Apply impulse to the body.
	virtual void ApplyImpulse(const vec3& impulse, const vec3& point) = 0;
	/// Apply force to the body.
	virtual void ApplyForce(const vec3& force, const vec3& point) = 0;

	/// Activate body.
	virtual void Activate() = 0;
	/// Disable deactivation.
	virtual void DisableDeactivation() = 0;

	META_DECLARE_CLASS(RigidBody);
};

END_INANITY_PHYSICS

#endif
