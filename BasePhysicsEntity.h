#pragma once

#include "DragonSector.h"
#include <vector>

class BasePhysicsEntity
{
private:
	// if anything needed to be private it would go here but its more of a pain then its worth at the moment
public:
	int mI_PhysicsType;
	int mI_PrimitiveType;
	//default constuctor
	BasePhysicsEntity(){
		m_Position = DragonXVector3(0.0f,0.0f,0.0f);
		m_LinearVelocity= DragonXVector3(0.0f,0.0f,0.0f);
		m_LinearForceAccum= DragonXVector3(0.0f,0.0f,0.0f);
		m_AngularForceAccum=DragonXVector3(0.0f,0.0f,0.0f);
		m_RotationalVelocity=DragonXVector3(0.0f,0.0f,0.0f);
		m_LastFrameAccel=DragonXVector3(0.0f,0.0f,0.0f);
		m_LinearAccel= DragonXVector3(0.0f,0.0f,0.0f);
		m_QuatRot=DragonXQuaternion(0.0f,0.0f,0.0f,1.0f);
		m_InverseMass=1.0f/2.0f;
		m_LinearDampening=0.95f;
		m_AngularDampening=0.95f;
		m_motion=0.0f;
		m_Mass=2.0f;
		m_isAwake=true;
		m_CanSleep=true;
		m_isCollisionResolutionOn=true;
		m_isCollisionDetectionOn=true;
	}
	// virtual destructor allows dervied objects to be deleted
	virtual~BasePhysicsEntity(){
	}
	// all data members are public so that the builder patttern can be used to change the various attributes though mass and inverse mass are best done through their
	// accessor mutator methods

	// this is the position of the object it is also the position of center of mass which ill assume to be the geometric center of all objects in this engine if this is not the case it could be implimented
	// fairly easily
	DragonXVector3 m_Position;
	// this is the linear velocity its used to update the position of the object
	DragonXVector3 m_LinearVelocity;
	// rotational velocity used to update quaternion it represents the velocity in each local axis
	DragonXVector3 m_RotationalVelocity;
	// the inverse inertia tensor in local space this is never used outside of obtaining the inverse inertia tensor in world cooridinates
	DragonXMatrix inverseInertiaTensor;
	// this is the inverse inertia tensor that is used basically exclusively
	DragonXMatrix inverseInertiaTensorWorld;
	// the transform matrix holds the position as well as the rotation vectors in all axis
	DragonXMatrix TransformMatrix;
	// this holds the amount of acceleration that was aquired last frame and is used in the sleep optimization
	DragonXVector3 m_LastFrameAccel;
	// this is a force accumulator for linear forces like gravity
	DragonXVector3 m_LinearForceAccum;
	// the angular force accumulator like linear only this is for making an object rotate or spin it can also be called a torque accumulator
	// though my spelling is pretty bad so i just stick with angular
	DragonXVector3 m_AngularForceAccum;
	// this is the linear acceleration mostly unused as far as im conscerned unless your trying to do some simulation work
	// i primarily use it as an easy way to add gravity in a direction typically down
	DragonXVector3 m_LinearAccel;
	// this is the quaternion that holds the orientation of the object
	DragonXQuaternion m_QuatRot;
	// this is the inverse mass its stored because mass is almost never used outside of the original calculation
	// of the inverse inertia tensor in body space
	real m_InverseMass;
	// linear dampening this is used do to numerical instability basically with out this and its associate angular dampening we would be in a vacum which may be useful in space
	// however that would mean objects would likely never fall asleep which will hurt performance
	real m_LinearDampening;
	// see above this just keeps the object from spinning forever
	real m_AngularDampening;
	// the mass is stored because although it is rarely used it is used in gravitational calculations
	real m_Mass;
	// this is the amount of motion that occured for this object this frame its used in conjunction with last frame accel to aid in putting objects that
	// have no reason for being awake to sleep
	real m_motion;
	// simple boolean value if this is true an object can sleep if false it cant sleep so use this as false sparingly to aid performance
	bool m_CanSleep;
	bool m_isAwake;
	// a boolean that determines wether collision resolution is on or off for the individual object
	// this will be used for trigger volumes in a later build in which we want to detect a collision
	// which will trigger the volume but we dont want the object that triggered the volume to imedeately be kicked out
	// of the volume which would ultimately defeat the purpose of the trigger volume
	bool m_isCollisionResolutionOn;
	// this is used to turn off collision detection for a specific entity
	// theoretical uses are particles and other effects that you want general physics on
	// such as movement or forces as maybe the case with explosions and particles being used for the mushroom cloud
	// effect that is already built in atm i mostly use it for debugging i create physics entities with a fixed position and turn off their collision detection so that
	// they dont get considered in my collision detector / contact generator as far as what im usung em for in debugging im showing the contact point and the normal
	bool m_isCollisionDetectionOn;
	//
	// this is simply is the object used for spheres and the broad phase collision detection for other primitives if its not over loaded its not needed by the primitive and will return 0.
	virtual real GetRadius()
	{
		return (real)0.0;
	}
	//this is used only for planes which will have it overloaded if the object doesnt then it will just return 0 because its not a plane
	virtual real GetDistFromOrigin()
	{
		return (real)0.0;
	}
	//another function just for planes
	virtual DragonXVector3 GetNormal()
	{
		return DragonXVector3(0.0f,0.0f,0.0f);
	}
	//used to get half sizes for obb
	virtual DragonXVector3 GetHalfSizes()
	{
		return DragonXVector3(0.0f,0.0f,0.0f);
	}
	// used to set halfsizes this is used in the kdop object so that it can compute the extents then set them
	virtual void setHalfSizes(DragonXVector3 )
	{
	}
	// used for 26 dops
	// this is used by the 26 dop to take in a list of verts that are generated from a mesh
	// it has no function for other physics objects
	virtual void setVerts(std::vector<DragonXVector3*>& verts)
	{
	}
	// also used by the 26 dops object to recompute the min n max along its various axis
	virtual void computeKDops()
	{
	}
	// the rest of the 26 dop functions ill talk about in the 26 dop class stuff
	virtual real getMin(int idx)
	{
		return -1;
	}
	virtual real getMax(int idx)
	{
		return -1;
	}
	virtual int getAxis(DragonXVector3 n)
	{
		return -1;
	}
	virtual std::vector<DragonXVector3*> getVerts()
	{
		std::vector<DragonXVector3*> blah;
		return blah;
	}
	virtual DragonXVector3 getAxisVector(int i)
	{
		DragonXVector3 blah;
		return blah;
	}
	// end of 26 dops stuff
	virtual std::vector<DragonSector*> GetSectors()
	{
		std::vector<DragonSector*> blah;
		return blah;
	}
	virtual void SetSectors(std::vector<DragonSector*>& sectors)
	{
	}
	// this is the main update method that updates internal variables such as position and orientation
	// also uses the force accums to update the velocities after using them it will clear the accumulators
	void Update(real dt);
	void UpdateMatricies();//updates internal matrices called every update
	// this will set the mass and inverse mass however note that the inertia tensor will not be changed
	// this is because the tensor is a matrix whose values depend on the shape that the physics entity represents
	// because the base physics entity knows nothing about its shape nor needs to the inertia tensor cant be updated from here
	// however if it is required i suppose it could be possible to have an overloaded method that can be called internally
	// to update the tensor
	void setMass(real mass);
	// this just gets the mass nothing fancy here side from returning a large number if the inverse mass is zero
	real getMass();
	void setInverseMass(real InvMass);//set this 2 zero for an object with infinite mass
	// this gets the inverse mass which is used for many physics based calculations
	real getInverseMass() const;
	// this returns  false if the inverse mass is zero and consequently the object has infinite mass other wise it has finite mass
	bool hasFiniteMass();
	void getInertiaTensor(DragonXMatrix *inverseMatrix)const;//pass in pointer to a matrix and it will be set to the inertia tensor expressed in the body's local space
	DragonXMatrix getInertiaTensor()const;//returns a copy of the inertia tensor matrix still in the body's local space
	void getInertiaTensorWorld(DragonXMatrix *m)const;// returns the inertia tensor in world space
	DragonXMatrix getInertiaTensorWorld()const;//returns a copy of the inertial tensor in world space
	void getInverseInertiaTensor(DragonXMatrix *m)const;//copies current inverse inertia tensor onto the give matrix in body local space
	DragonXMatrix getInverseInertiaTensor()const;// you get the point
	void getInverseInertiaTensorWorld(DragonXMatrix *m)const;//pass in pointer its now inverse inertia tensor in world space
	DragonXMatrix getInverseInertiaTensorWorld()const; // inversive tensor in world
	void setDampening(const real linear,const real angular);//set both linear and angular dampening in one call
	void setLinearDampening(const real linearDampening);//set linear dampening
	real getLinearDampening() const;//take a guess
	void setAngularDampening(const real angular); // sets angular dampening
	real getAngularDampening() const; // yup it returns the angular dampening
	void setPosition(const DragonXVector3 &pos);//sets position to the given vector
	void setPosition(const real x, const real y, const real z); // sets the position to the new vector generated by the given parameters
	void getPosition(DragonXVector3 *pos) const; //changes the given vector3 to the position
	DragonXVector3 getPosition() const; // returns the current position vector3
	void setOrientation(const DragonXQuaternion &orientation);//sets the orientation of the object  the quaternion needn't be normalized and can be zero this will
	// automatically make a zero rotation  quaternion
	DragonXQuaternion getOrientation() const; //returns the orientation in quaternion form
	void getOrientation(DragonXMatrix *m) const;//fills the given matrix with a transformation representing its orientation @note transforming a direction vector by this matrix turns it from local to world
	void getTransform(DragonXMatrix *m) const;// fills matrix with a transform representing body's position and orientation NOTE: transforming a vector by this matrix will turn it from body to world space
	DragonXMatrix getTransform() const; // returns the transform matrix 4 the object
	/**
	* Converts the given point from world space into the body's
	* local space.
	*
	* @param point The point to covert, given in world space.
	*
	* @return The converted point, in local space.
	*/
	DragonXVector3 getPointInLocalSpace(const DragonXVector3 &point) const;

	/**
	* Converts the given point from world space into the body's
	* local space.
	*
	* @param point The point to covert, given in local space.
	*
	* @return The converted point, in world space.
	*/
	DragonXVector3 getPointInWorldSpace(const DragonXVector3 &point) const;

	/**
	* Converts the given direction from world space into the
	* body's local space.
	*
	* @note When a direction is converted between frames of
	* reference, there is no translation required.
	*
	* @param direction The direction to covert, given in world
	* space.
	*
	* @return The converted direction, in local space.
	*/
	DragonXVector3 getDirectionInLocalSpace(const DragonXVector3 &direction) const;

	/**
	* Converts the given direction from world space into the
	* body's local space.
	*
	* @note When a direction is converted between frames of
	* reference, there is no translation required.
	*
	* @param direction The direction to covert, given in local
	* space.
	*
	* @return The converted direction, in world space.
	*/
	DragonXVector3 getDirectionInWorldSpace(const DragonXVector3 &direction) const;
	/**
	* Sets the velocity of the rigid body.
	*
	* @param velocity The new velocity of the rigid body. The
	* velocity is given in world space.
	*/
	void setVelocity(const DragonXVector3 &velocity);

	/**
	* Sets the velocity of the rigid body by component. The
	* velocity is given in world space.
	*
	* @param x The x coordinate of the new velocity of the rigid
	* body.
	*
	* @param y The y coordinate of the new velocity of the rigid
	* body.
	*
	* @param z The z coordinate of the new velocity of the rigid
	* body.
	*/
	void setVelocity(const real x, const real y, const real z);

	/**
	* Fills the given vector with the velocity of the rigid body.
	*
	* @param velocity A pointer to a vector into which to write
	* the velocity. The velocity is given in world local space.
	*/
	void getVelocity(DragonXVector3 *velocity) const;

	/**
	* Gets the velocity of the rigid body.
	*
	* @return The velocity of the rigid body. The velocity is
	* given in world local space.
	*/
	DragonXVector3 getVelocity() const;

	/**
	* Applies the given change in velocity.
	*/
	void addVelocity(const DragonXVector3 &deltaVelocity);

	/**
	* Sets the rotation of the rigid body.
	*
	* @param rotation The new rotation of the rigid body. The
	* rotation is given in world space.
	*/
	void setRotation(const DragonXVector3 &rotation);

	/**
	* Sets the rotation of the rigid body by component. The
	* rotation is given in world space.
	*
	* @param x The x coordinate of the new rotation of the rigid
	* body.
	*
	* @param y The y coordinate of the new rotation of the rigid
	* body.
	*
	* @param z The z coordinate of the new rotation of the rigid
	* body.
	*/
	void setRotation(const real x, const real y, const real z);

	/**
	* Fills the given vector with the rotation of the rigid body.
	*
	* @param rotation A pointer to a vector into which to write
	* the rotation. The rotation is given in world local space.
	*/
	void getRotation(DragonXVector3 *rotation) const;

	/**
	* Gets the rotation of the rigid body.
	*
	* @return The rotation of the rigid body. The rotation is
	* given in world local space.
	*/
	DragonXVector3 getRotation() const;

	/**
	* Applies the given change in rotation.
	*/
	void addRotation(const DragonXVector3 &deltaRotation);

	/**
	* Returns true if the body is awake and responding to
	* integration.
	*
	* @return The awake state of the body.
	*/
	bool getAwake() const
	{
		return m_isAwake;
	}

	/**
	* Sets the awake state of the body. If the body is set to be
	* not awake, then its velocities are also cancelled, since
	* a moving body that is not awake can cause problems in the
	* simulation.
	*
	* @param awake The new awake state of the body.
	*/
	void setAwake(const bool awake=true);

	/**
	* Returns true if the body is allowed to go to sleep at
	* any time.
	*/
	bool getCanSleep() const
	{
		return m_CanSleep;
	}

	/**
	* Sets whether the body is ever allowed to go to sleep. Bodies
	* under the player's control, or for which the set of
	* transient forces applied each frame are not predictable,
	* should be kept awake.
	*
	* @param canSleep Whether the body can now be put to sleep.
	*/
	void setCanSleep(const bool canSleep=true);

	/*@}*/

	/**
	* @name Retrieval Functions for Dynamic Quantities
	*
	* These functions provide access to the acceleration
	* properties of the body. The acceleration is generated by
	* the simulation from the forces and torques applied to the
	* rigid body. Acceleration cannot be directly influenced, it
	* is set during integration, and represent the acceleration
	* experienced by the body of the previous simulation step.
	*/
	/*@{*/

	/**
	* Fills the given vector with the current accumulated value
	* for linear acceleration. The acceleration accumulators
	* are set during the integration step. They can be read to
	* determine the rigid body's acceleration over the last
	* integration step. The linear acceleration is given in world
	* space.
	*
	* @param linearAcceleration A pointer to a vector to receive
	* the linear acceleration data.
	*/
	void getLastFrameAcceleration(DragonXVector3 *linearAcceleration) const;

	/**
	* Gets the current accumulated value for linear
	* acceleration. The acceleration accumulators are set during
	* the integration step. They can be read to determine the
	* rigid body's acceleration over the last integration
	* step. The linear acceleration is given in world space.
	*
	* @return The rigid body's linear acceleration.
	*/
	DragonXVector3 getLastFrameAcceleration() const;
	/**
	* @name Force, Torque and Acceleration Set-up Functions
	*
	* These functions set up forces and torques to apply to the
	* rigid body.
	*/

	/**
	* Clears the forces and torques in the accumulators. This will
	* be called automatically after each update.
	*/
	void clearAccumulators();

	/**
	* Adds the given force to centre of mass of the rigid body.
	* The force is expressed in world-coordinates.
	*
	* @param force The force to apply.
	*/
	void addForce(const DragonXVector3 &force);

	/**
	* Adds the given force to the given point on the rigid body.
	* Both the force and the
	* application point are given in world space. Because the
	* force is not applied at the center of mass, it may be split
	* into both a force and torque.
	*
	* @param force The force to apply.
	*
	* @param point The location at which to apply the force, in
	* world-coordinates.
	*/
	void addForceAtPoint(const DragonXVector3 &force, const DragonXVector3 &point);

	/**
	* Adds the given force to the given point on the rigid body.
	* The direction of the force is given in world coordinates,
	* but the application point is given in body space. This is
	* useful for spring forces, or other forces fixed to the
	* body.
	*
	* @param force The force to apply.
	*
	* @param point The location at which to apply the force, in
	* body-coordinates.
	*/
	void addForceAtBodyPoint(const DragonXVector3 &force, const DragonXVector3 &point);

	/**
	* Adds the given torque to the rigid body.
	* The force is expressed in world-coordinates.
	*
	* @param torque The torque to apply.
	*/
	void addTorque(const DragonXVector3 &torque);

	/**
	* Sets the constant acceleration of the rigid body.
	*
	* @param acceleration The new acceleration of the rigid body.
	*/
	void setAcceleration(const DragonXVector3 &acceleration);

	/**
	* Sets the constant acceleration of the rigid body by component.
	*
	* @param x The x coordinate of the new acceleration of the rigid
	* body.
	*
	* @param y The y coordinate of the new acceleration of the rigid
	* body.
	*
	* @param z The z coordinate of the new acceleration of the rigid
	* body.
	*/
	void setAcceleration(const real x, const real y, const real z);

	/**
	* Fills the given vector with the acceleration of the rigid body.
	*
	* @param acceleration A pointer to a vector into which to write
	* the acceleration. The acceleration is given in world local space.
	*/
	void getAcceleration(DragonXVector3 *acceleration) const;

	/**
	* Gets the acceleration of the rigid body.
	*
	* @return The acceleration of the rigid body. The acceleration is
	* given in world local space.
	*/
	DragonXVector3 getAcceleration() const;

	virtual void SetMassAndInverseTensor(real newMass)
	{
	}
};
