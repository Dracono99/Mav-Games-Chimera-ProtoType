#pragma once

#include "BaseGameEntity.h"

class DragonContact
{
private:

public:
	//constructor sets up some base things for use later
	DragonContact();
	// destructor what do you think its used 4
	~DragonContact();
	// this is an array of base game pointers it holds the 2 entities that are contacting
	BaseGameEntity* m_aEntitiesContacted[2];
	// 3d vector that holds the contact point
	DragonXVector3 m_vContactPoint;
	//the contact normal
	DragonXVector3 m_vContactNormal;
	// the coefficient of restitution or bounciness of the collision between 0.0 and 1.0 , 1.0 means no energy is lost 0.0 means all energy is lost
	//if you want to be funny for a bit till it crashes use greater than 1.0 which means collisions generate energy
	real m_rRestitution;
	// the friction that will be used for the game
	real m_rFriction;
	//holds depth of penetration
	real m_rPenetration;
	//this is called after the constructor to set various data like the contacting bodies and the properties associated with the collision like restitution or bounciness
	void SetContactData(BaseGameEntity* entity1,BaseGameEntity* entity2,real restituion);
	// this is a transform matrix that converts from contact coordinates to world and back
	DragonXMatrix contactToWorld;
	// this is the closing velocity or the combined velocity of the objects
	DragonXVector3 contactVelocity;
	// this holds the change in velocity required for the contact to be resolved
	// basically the top half of the impulse calculation formula
	real desiredDeltaVelocity;
	//holds world space position of the point of contact relative to the center of each body
	DragonXVector3 relitiveContactPosition[2];
	// calculates internal data this is usually called automatically before the resolver resolves this contact
	// there for it should not need to be called manually
	void calculateInternals(real dt);
	// reverses the contacting bodies so body 0 and 1 in the array are swapped
	// you should calculate the internals again after doing this as its not done automatically
	void swapBodies();
	// basically if you collide a sleeping object wake it up but if your also asleep don't bother
	void matchAwakeState();
	/**
	* Calculates and sets the internal value for the desired delta
	* velocity. aka the expected velocity change
	*/
	void calculateDesiredDeltaVelocity(real dt);
	//calculates and returns the velocity of the contact point on the given body
	DragonXVector3 calculateLocalVelocity(unsigned entityIndex, real dt);
	// this calculates and applies impulse based collision resolution to the velocities of the objects
	void applyVelocityChange(DragonXVector3 velocityChange[2],	DragonXVector3 angularChange[2]);
	// performs an inertia weighted penetration resolution  of this contact
	void applyPositionChange(DragonXVector3 linearChange[2],DragonXVector3 angularChange[2],real penetration);
	// creates an orthanormal contact basis that mostly just helps in friction but has some other less important uses
	// though its primary function is to change between world and contact coordinates
	void calculateContactBasis();
	/**
	this will calculate the impulse that will be used for velocity resolution
	*/
	DragonXVector3 calculateFrictionlessImpulse(DragonXMatrix *inverseInertiaTensor);
	// calculate friction impulse more tedius and more calculations but it will look better
	DragonXVector3 calculateFrictionImpulse(DragonXMatrix* inverseInertiaTensor);
};
