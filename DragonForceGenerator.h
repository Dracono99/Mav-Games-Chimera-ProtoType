#pragma once
#include "BaseGameEntity.h"

class DragonForceGenerator//pure virtual class that all force generators will inherit from
{
public:
	// the boolean return is true or false if true it will be added to the force engines list of forces to delete and remove from the
	//current registry after all the entities have been updated, oh false means it will not be deleted or removed
	virtual bool updateForce(BaseGameEntity* entity, real dt)=0;
	virtual ~DragonForceGenerator(){}
};

class GravityFromPoint : public DragonForceGenerator
{
	const static double mGravityConstant;
	DragonXVector3 mPos;//position of gravity
	real m_MassOfPoint;//mass of the point
public:
	// creates the generator for a gravity force from a point
	GravityFromPoint(const DragonXVector3 &Position, real mass);
	~GravityFromPoint();
	//applies the update
	virtual bool updateForce(BaseGameEntity* entity, real dt);
};

// this generates a force of gravity between two entities
class GravityBetweenEntities : public DragonForceGenerator
{
	const static double mGravityConstant;
	BaseGameEntity* m_entity1;//the first entity of the pair of entities that have gravity
	BaseGameEntity* m_entity2; // the other one
public:
	//creates the force
	GravityBetweenEntities(BaseGameEntity* entity1,BaseGameEntity* entity2);
	~GravityBetweenEntities();
	//updates the force will decide internally which one is which
	virtual bool updateForce(BaseGameEntity* entity, real dt);
};
// this is a fancy explosion force
class Explosion : public DragonForceGenerator
{
	// how long the explosion has been in operation
	real m_timePassed;
public:
	// properties for the explosion
	// public for alteration
	// this is the origin of the blast
	DragonXVector3 m_pointOfDetonation;

	// radius up to which the objects will implode for stage one of the explosion
	real m_implosionMaxRadius;

	// radius at which the objects are not affected by the implosion
	real m_implosionMinRadius;

	// implosion duration
	// the length of time spent imploding before the concussive phase begins
	real m_implosionDuration;

	// implosion force should be relatively small to avoid sucking entities in past the origin of the blast n out the other side
	real m_implosionForce;

	// the speed at which the shock wave is traveling this is related to
	// the thickness below in the relationship
	// thickness >= speed * minimum frame duration
	real m_shockWaveSpeed;

	// a shock wave applies its force over a range of distances
	// this controls how thick, faster waves require larger thicknesses
	real m_shockWaveThickness;

	// this is the force applied to the very center of the blast
	real m_peakConcussiveForce;

	// length of time the concussive wave is active
	// closer we get to this the less powerful the blast
	real m_concussionDuration;

	// this is the peak force for the stationary objects in
	// the center of the convection chimney
	// calculations 4 this force are the same as peak concussive force
	real m_peakConvectionForce;

	// the radius of the chimney cylinder in the xz plane
	real m_chmineyRadius;

	// maximum height of the mushroom cloud aka chimney
	real m_chimneyHeight;

	// this is the length of time for the chimney effect
	// this is typically the longest as the heat from the blast typically out lives the shock wave it self
	real m_convectionDuration;

public:

	// creates a default explosion with sensible default values
	Explosion();
	~Explosion();

	// applies the force to the given entity
	virtual bool updateForce(BaseGameEntity* entity, real dt);
};

class Thrust : public DragonForceGenerator
{
	//length of time 4 the trust to be active in seconds
	// 0 for a one time thrust think bullet or rock
	// - 1 will be a continuous thrust think rocket with no steering and infinite gas
	real m_durationOfThrust;

	// this is drag or the mpg or whatever the magnitude of the thrust will be decreased by this based on dt so
	// the magnitude will be decreased by this amount every second so if mpg you need a new car
	// side note you can make this negative for an increase in thrust till the duration of the thrust force has been reached
	real m_decrement;

	// this is the magnitude or strength of the thrust
	real m_magnitudeOfTrust;

	// this is the direction of the thrust
	// they are stored separately so the magnitude can be decremented
	// and then integrated into the force update by scaling the direction
	DragonXVector3 m_direction; // note this should be normalized if its not thats ok it will be normalized in the constructor

public:
	// default constructor will fill out the values with reasonable values
	Thrust();
	// this is the parameterized constructor
	Thrust(real duration,real decrement,real magnitude,DragonXVector3 direction);
	~Thrust();
	// updates the entity with the force
	virtual bool updateForce(BaseGameEntity* entity, real dt);
};

class AnchoredSpringGenerator : public DragonForceGenerator
{
private:
	// holds spring constant
	real m_SpringConstant;
	// holds rest length
	real m_RestLength;
	// holds anchor pos
	DragonXVector3 m_AnchorPosition;
	// holds offset for the object
	DragonXVector3 m_OffSet;
public:
	AnchoredSpringGenerator();
	AnchoredSpringGenerator(real constant, real restLength, DragonXVector3& anchorPos);
	~AnchoredSpringGenerator();
	void setOffSet(DragonXVector3& offset);
	virtual bool updateForce(BaseGameEntity* entity, real dt);
};

class AnchoredBungeeGenerator : public DragonForceGenerator
{
	// holds spring constant
	real m_SpringConstant;
	// holds rest length
	real m_RestLength;
	// holds anchor pos
	DragonXVector3 m_AnchorPosition;
	// holds offset for the object
	DragonXVector3 m_OffSet;
public:
	AnchoredBungeeGenerator();
	AnchoredBungeeGenerator(real constant, real restLength, DragonXVector3& anchorPos);
	~AnchoredBungeeGenerator();
	void setOffSet(DragonXVector3& offset);
	virtual bool updateForce(BaseGameEntity* entity, real dt);
};

class ConnectingSpringForceGenerator : public DragonForceGenerator
{
private:
	// entity at the other end of the spring
	BaseGameEntity* m_OtherEntity;
	// spring constant
	real m_SpringConstant;
	// rest length
	real m_RestLength;
	// off set in 3 dimensions 0,0,0 is no offset
	// the offset will be subtracted from the position of the updated entity
	DragonXVector3 m_OffsetForUpdatedEntity;
	// offset for other
	DragonXVector3 m_OffsetForOther;
public:
	ConnectingSpringForceGenerator(real constant, real restLength, BaseGameEntity* otherEntity);
	~ConnectingSpringForceGenerator();
	void setOffSetOther(DragonXVector3& offset);
	void setOffSetMain(DragonXVector3& offset);
	virtual bool updateForce(BaseGameEntity* entity, real dt);
};

class ConnectingBungeeForceGenerator : public DragonForceGenerator
{
private:
	// entity at the other end of the spring
	BaseGameEntity* m_OtherEntity;
	// spring constant
	real m_SpringConstant;
	// rest length
	real m_RestLength;
	// off set in 3 dimensions 0,0,0 is no offset
	// the offset will be subtracted from the position of the updated entity
	DragonXVector3 m_OffsetForUpdatedEntity;
	// offset for other
	DragonXVector3 m_OffsetForOther;
public:
	ConnectingBungeeForceGenerator();
	ConnectingBungeeForceGenerator(real constant, real restLength, BaseGameEntity* otherEntity);
	~ConnectingBungeeForceGenerator();
	void setOffSetOther(DragonXVector3& offset);
	void setOffSetMain(DragonXVector3& offset);
	virtual bool updateForce(BaseGameEntity* entity, real dt);
};