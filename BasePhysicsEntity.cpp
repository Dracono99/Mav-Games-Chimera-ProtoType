#include "BasePhysicsEntity.h"
#include <cassert>

real sleepEpsilon=0.75f;
const real REAL_MAX = 99999999.9f;

// make wire frame bounding box for dynamic meshes
// make wire frames for the octrees , pause engine hit button to render octrees and of course pause the update on the entities
// different masses and proportional sizes for collision

/**
* Inline function that creates a transform matrix from a
* position and m_QuatRot.
*/
static inline void _calculateTransformMatrix(DragonXMatrix &TransformMatrix,
											 const DragonXVector3 &position,
											 const DragonXQuaternion &m_QuatRot)
{
	TransformMatrix._11 = 1-2*m_QuatRot.y*m_QuatRot.y-2*m_QuatRot.z*m_QuatRot.z;
	TransformMatrix._12 = 2*m_QuatRot.x*m_QuatRot.y -2*m_QuatRot.w*m_QuatRot.z;
	TransformMatrix._13 = 2*m_QuatRot.x*m_QuatRot.z +2*m_QuatRot.w*m_QuatRot.y;
	TransformMatrix._14 = position.x;

	TransformMatrix._21 = 2*m_QuatRot.x*m_QuatRot.y +2*m_QuatRot.w*m_QuatRot.z;
	TransformMatrix._22 = 1-2*m_QuatRot.x*m_QuatRot.x-2*m_QuatRot.z*m_QuatRot.z;
	TransformMatrix._23 = 2*m_QuatRot.y*m_QuatRot.z -2*m_QuatRot.w*m_QuatRot.x;
	TransformMatrix._24 = position.y;

	TransformMatrix._31 = 2*m_QuatRot.x*m_QuatRot.z -2*m_QuatRot.w*m_QuatRot.y;
	TransformMatrix._32 = 2*m_QuatRot.y*m_QuatRot.z +2*m_QuatRot.w*m_QuatRot.x;
	TransformMatrix._33 = 1-2*m_QuatRot.x*m_QuatRot.x-2*m_QuatRot.y*m_QuatRot.y;
	TransformMatrix._34 = position.z;
}

/*
* --------------------------------------------------------------------------
* FUNCTIONS DECLARED IN HEADER:
* --------------------------------------------------------------------------
*/
void BasePhysicsEntity::UpdateMatricies()
{
	m_QuatRot.normalize();

	// Calculate the transform matrix for the body.
	_calculateTransformMatrix(TransformMatrix, m_Position, m_QuatRot);
	// here i create a temporary matrix that i will use for concatinating my matricies
	// so that i can obtain my inverse oh tensor in world space
	DragonXMatrix m;
	// what im doing here is basically creating a pure rotation matrix from my transform matrix by obtaining each axis of rotation
	m.setComponents(TransformMatrix.getAxisVector(0),TransformMatrix.getAxisVector(1),TransformMatrix.getAxisVector(2));
	// not order matters so first we get our local tensor to rotate correctly in local space although only temporarily we then multiply that result by the non inverse of the pure
	// rotation matrix which results in our tensor in world space
	inverseInertiaTensorWorld= m * inverseInertiaTensor * m.getInverse();
}

void BasePhysicsEntity::Update(real duration)
{
	if (!m_isAwake) return;

	// Calculate linear acceleration from force inputs.
	m_LastFrameAccel = m_LinearAccel;
	m_LastFrameAccel+=m_LinearForceAccum*m_InverseMass;

	// Calculate angular acceleration from torque inputs.
	DragonXVector3 angularAcceleration =
		inverseInertiaTensorWorld.transform(m_AngularForceAccum);

	// Adjust velocities
	// Update linear velocity from both acceleration and impulse.
	m_LinearVelocity+=m_LastFrameAccel*duration;

	// Update angular velocity from both acceleration and impulse.
	m_RotationalVelocity+=angularAcceleration*duration;

	// Impose drag.
	m_LinearVelocity *= pow(m_LinearDampening, duration);
	m_RotationalVelocity *= pow(m_AngularDampening, duration);

	// Adjust positions
	// Update linear position.
	m_Position+=m_LinearVelocity*duration;

	// Update angular position.
	m_QuatRot.addScaledVectorDrX(m_RotationalVelocity, duration);

	// Normalise the m_QuatRot, and update the matrices with the new
	// position and m_QuatRot
	UpdateMatricies();

	// Clear accumulators.
	clearAccumulators();

	// Update the kinetic energy store, and possibly put the body to
	// sleep.
	if (m_CanSleep) {
		real currentMotion = m_LinearVelocity*m_LinearVelocity +
			m_RotationalVelocity*m_RotationalVelocity;

		real bias = (real)pow(0.5, duration);
		m_motion = bias*m_motion + (1-bias)*currentMotion;

		if (m_motion < sleepEpsilon) setAwake(false);
		else if (m_motion > 10 * sleepEpsilon) m_motion = 10 * sleepEpsilon;
	}
}

void BasePhysicsEntity::setMass(const real mass)
{
	assert(mass != 0);
	m_InverseMass = ((real)1.0)/mass;
}

real BasePhysicsEntity::getMass()
{
	if (m_InverseMass == 0) {
		return REAL_MAX;
	} else {
		return ((real)1.0)/m_InverseMass;
	}
}

void BasePhysicsEntity::setInverseMass(const real inverseMass)
{
	m_InverseMass = inverseMass;
	if (m_InverseMass == 0) {
		m_Mass= REAL_MAX;
	}
}

real BasePhysicsEntity::getInverseMass() const
{
	return m_InverseMass;
}

bool BasePhysicsEntity::hasFiniteMass()
{
	return m_InverseMass > 0.0f;
}

void BasePhysicsEntity::getInertiaTensor(DragonXMatrix *inertiaTensor) const
{
	*inertiaTensor=inverseInertiaTensor.getInverse();
}

DragonXMatrix BasePhysicsEntity::getInertiaTensor() const
{
	DragonXMatrix it;
	getInertiaTensor(&it);
	return it;
}

void BasePhysicsEntity::getInertiaTensorWorld(DragonXMatrix *m) const
{
	*m=inverseInertiaTensorWorld.getInverse();
}

DragonXMatrix BasePhysicsEntity::getInertiaTensorWorld() const
{
	DragonXMatrix it;
	getInertiaTensorWorld(&it);
	return it;
}

void BasePhysicsEntity::getInverseInertiaTensor(DragonXMatrix *m) const
{
	*m = inverseInertiaTensor;
}

DragonXMatrix BasePhysicsEntity::getInverseInertiaTensor() const
{
	return inverseInertiaTensor;
}

void BasePhysicsEntity::getInverseInertiaTensorWorld(DragonXMatrix *m) const
{
	*m = inverseInertiaTensorWorld;
}

DragonXMatrix BasePhysicsEntity::getInverseInertiaTensorWorld() const
{
	return inverseInertiaTensorWorld;
}

void BasePhysicsEntity::setDampening(const real linearDamping,const real angularDamping)
{
	m_LinearDampening = linearDamping;
	m_AngularDampening = angularDamping;
}

void BasePhysicsEntity::setLinearDampening(const real linearDamping)
{
	m_LinearDampening = linearDamping;
}

real BasePhysicsEntity::getLinearDampening() const
{
	return m_LinearDampening;
}

void BasePhysicsEntity::setAngularDampening(const real angularDamping)
{
	m_AngularDampening = angularDamping;
}

real BasePhysicsEntity::getAngularDampening() const
{
	return m_AngularDampening;
}

void BasePhysicsEntity::setPosition(const DragonXVector3 &position)
{
	m_Position = position;
}

void BasePhysicsEntity::setPosition(const real x, const real y, const real z)
{
	m_Position.x = x;
	m_Position.y = y;
	m_Position.z = z;
}

void BasePhysicsEntity::getPosition(DragonXVector3 *position) const
{
	*position = m_Position;
}

DragonXVector3 BasePhysicsEntity::getPosition() const
{
	return m_Position;
}

void BasePhysicsEntity::setOrientation(const DragonXQuaternion &orientation)
{
	m_QuatRot = orientation;
	m_QuatRot.normalize();
}

DragonXQuaternion BasePhysicsEntity::getOrientation() const
{
	return m_QuatRot;
}

void BasePhysicsEntity::getOrientation(DragonXMatrix *matrix) const
{
	*matrix=TransformMatrix;
}

void BasePhysicsEntity::getTransform(DragonXMatrix *transform) const
{
	memcpy(transform, &TransformMatrix, sizeof(DragonXMatrix));
}

DragonXMatrix BasePhysicsEntity::getTransform() const
{
	return TransformMatrix;
}

DragonXVector3 BasePhysicsEntity::getPointInLocalSpace(const DragonXVector3 &point) const
{
	return TransformMatrix.TransformInverse(point);
}

DragonXVector3 BasePhysicsEntity::getPointInWorldSpace(const DragonXVector3 &point) const
{
	return TransformMatrix.transform(point);
}

DragonXVector3 BasePhysicsEntity::getDirectionInLocalSpace(const DragonXVector3 &direction) const
{
	return TransformMatrix.TransformInverseDirection(direction);
}

DragonXVector3 BasePhysicsEntity::getDirectionInWorldSpace(const DragonXVector3 &direction) const
{
	return TransformMatrix.TransformDirection(direction);
}

void BasePhysicsEntity::setVelocity(const DragonXVector3 &velocity)
{
	m_LinearVelocity = velocity;
}

void BasePhysicsEntity::setVelocity(const real x, const real y, const real z)
{
	m_LinearVelocity.x = x;
	m_LinearVelocity.y = y;
	m_LinearVelocity.z = z;
}

void BasePhysicsEntity::getVelocity(DragonXVector3 *velocity) const
{
	*velocity = m_LinearVelocity;
}

DragonXVector3 BasePhysicsEntity::getVelocity() const
{
	return m_LinearVelocity;
}

void BasePhysicsEntity::addVelocity(const DragonXVector3 &deltaVelocity)
{
	m_LinearVelocity += deltaVelocity;
}

void BasePhysicsEntity::setRotation(const DragonXVector3 &rotation)
{
	m_RotationalVelocity = rotation;
}

void BasePhysicsEntity::setRotation(const real x, const real y, const real z)
{
	m_RotationalVelocity.x = x;
	m_RotationalVelocity.y = y;
	m_RotationalVelocity.z = z;
}

void BasePhysicsEntity::getRotation(DragonXVector3 *rotation) const
{
	*rotation = m_RotationalVelocity;
}

DragonXVector3 BasePhysicsEntity::getRotation() const
{
	return m_RotationalVelocity;
}

void BasePhysicsEntity::addRotation(const DragonXVector3 &deltaRotation)
{
	m_RotationalVelocity += deltaRotation;
}

void BasePhysicsEntity::setAwake(const bool awake)
{
	if (awake) {
		m_isAwake= true;

		// Add a bit of motion to avoid it falling asleep immediately.
		m_motion = sleepEpsilon*2.0f;
	} else {
		m_isAwake = false;
		m_LinearVelocity.clear();
		m_RotationalVelocity.clear();
	}
}

void BasePhysicsEntity::setCanSleep(const bool canSleep)
{
	m_CanSleep = canSleep;

	if (!canSleep && !m_isAwake) setAwake();
}

void BasePhysicsEntity::getLastFrameAcceleration(DragonXVector3 *acceleration) const
{
	*acceleration = m_LastFrameAccel;
}

DragonXVector3 BasePhysicsEntity::getLastFrameAcceleration() const
{
	return m_LastFrameAccel;
}

void BasePhysicsEntity::clearAccumulators()
{
	m_LinearForceAccum.clear();
	m_AngularForceAccum.clear();
}

void BasePhysicsEntity::addForce(const DragonXVector3 &force)
{
	m_LinearForceAccum += force;
	setAwake(true);
}

void BasePhysicsEntity::addForceAtBodyPoint(const DragonXVector3 &force,const DragonXVector3 &point)
{
	// Convert to coordinates relative to center of mass.
	DragonXVector3 pt = getPointInWorldSpace(point);
	addForceAtPoint(force, pt);
}

void BasePhysicsEntity::addForceAtPoint(const DragonXVector3 &force,const DragonXVector3 &point)
{
	// Convert to coordinates relative to center of mass.
	DragonXVector3 pt = point;
	pt -= m_Position;

	m_LinearForceAccum += force;
	m_AngularForceAccum += pt % force;

	setAwake(true);
}

void BasePhysicsEntity::addTorque(const DragonXVector3 &torque)
{
	m_AngularForceAccum += torque;
	setAwake(true);
}

void BasePhysicsEntity::setAcceleration(const DragonXVector3 &acceleration)
{
	m_LinearAccel = acceleration;
}

void BasePhysicsEntity::setAcceleration(const real x, const real y, const real z)
{
	m_LinearAccel.x = x;
	m_LinearAccel.y = y;
	m_LinearAccel.z = z;
}

void BasePhysicsEntity::getAcceleration(DragonXVector3 *acceleration) const
{
	*acceleration = m_LinearAccel;
}

DragonXVector3 BasePhysicsEntity::getAcceleration() const
{
	return m_LinearAccel;
}