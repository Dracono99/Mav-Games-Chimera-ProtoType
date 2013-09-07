#include "DragonForceGenerator.h"

const double GravityFromPoint::mGravityConstant=0.0006;
const double GravityBetweenEntities::mGravityConstant=0.6;

GravityFromPoint::GravityFromPoint(const DragonXVector3 &Position, real mass)
{
	mPos=Position;
	m_MassOfPoint=mass;
}
GravityFromPoint::~GravityFromPoint()
{
}
//applies the update
bool GravityFromPoint::updateForce(BaseGameEntity* entity, real dt)
{
	if(!entity->GetPhysicsPointer()->hasFiniteMass())
	{
		return false;
	}
	DragonXVector3 dir = mPos-entity->GetPhysicsPointer()->getPosition();
	real dist = dir.GetMagSquared();
	dir.Normalize();
	real force = (real)mGravityConstant*m_MassOfPoint*entity->GetPhysicsPointer()->getMass();
	force /= dist;
	dir*=force;
	entity->GetPhysicsPointer()->addForce(dir);
	if(force<0.0001)
	{
		return true;
	}
	else
	{
		return false;
	}
}

//creates the force
GravityBetweenEntities::GravityBetweenEntities(BaseGameEntity* entity1,BaseGameEntity* entity2)
{
	m_entity1=entity1;
	m_entity2=entity2;
}
GravityBetweenEntities::~GravityBetweenEntities()
{
}
//updates the force will decide internally which one is which
bool GravityBetweenEntities::updateForce(BaseGameEntity* entity, real dt)
{
	if(!entity->GetPhysicsPointer()->hasFiniteMass())
	{
		return false;
	}
	DragonXVector3 dir = entity->GetPhysicsPointer()->getPosition()-m_entity2->GetPhysicsPointer()->getPosition();
	real dist = dir.GetMagSquared();
	dir.Normalize();
	dir.invert();
	real force = (real)mGravityConstant*m_entity2->GetPhysicsPointer()->getMass() *entity->GetPhysicsPointer()->getMass();
	force /= dist;
	dir*=force;
	entity->GetPhysicsPointer()->addForce(dir);
	if(force<0.0001)
	{
		return true;
	}
	else
	{
		return false;
	}
}

// creates a default explosion with sensible default values
Explosion::Explosion()
{
	// how long the explosion has been in operation
	m_timePassed=0.0f;

	// properties for the explosion
	// public for alteration
	// this is the origin of the blast
	m_pointOfDetonation = DragonXVector3(0.0f,0.0f,0.0f);

	// radius up to which the objects will implode for stage one of the explosion
	m_implosionMaxRadius = 150.0f;

	// radius at which the objects are not affected by the implosion
	m_implosionMinRadius=75.0f;

	// implosion duration
	// the length of time spent imploding before the concussive phase begins
	m_implosionDuration = 0.5f;

	// implosion force should be relatively small to avoid sucking entities in past the origin of the blast n out the other side
	m_implosionForce = 8.0f;

	// the speed at which the shock wave is traveling this is related to
	// the thickness below in the relationship
	// thickness >= speed * minimum frame duration
	m_shockWaveSpeed=1000.0f;

	// a shock wave applies its force over a range of distances
	// this controls how thick, faster waves require larger thicknesses
	m_shockWaveThickness=300.0f;

	// this is the force applied to the very center of the blast
	m_peakConcussiveForce=2000.0f;

	// length of time the concussive wave is active
	// closer we get to this the less powerful the blast
	m_concussionDuration=1.0f;

	// this is the peak force for the stationary objects in
	// the center of the convection chimney
	// calculations 4 this force are the same as peak concussive force
	m_peakConvectionForce=500.0f;

	// the radius of the chimney cylinder in the xz plane
	m_chmineyRadius=200.0f;

	// maximum height of the mushroom cloud aka chimney
	m_chimneyHeight=500.0f;

	// this is the length of time for the chimney effect
	// this is typically the longest as the heat from the blast typically out lives the shock wave it self
	m_convectionDuration=2.0f;
}
Explosion::~Explosion()
{
}

// applies the force to the given entity
bool Explosion::updateForce(BaseGameEntity* entity, real dt)
{
	if(!entity->GetPhysicsPointer()->hasFiniteMass())
	{
		return true;
	}
	m_timePassed+=dt;
	if(m_implosionDuration>0.0f)
	{
		m_implosionDuration-=dt;
		DragonXVector3 dir = entity->GetPhysicsPointer()->getPosition()-m_pointOfDetonation;
		real dist=dir.GetMagnitude();
		if(dist<=m_implosionMaxRadius&&dist>=m_implosionMinRadius)
		{
			dir.Normalize();
			dir*=m_implosionForce;
			entity->GetPhysicsPointer()->addForce(dir);
		}
		return false;
	}
	else if(m_concussionDuration>0.0f)
	{
		m_concussionDuration-=dt;
		DragonXVector3 dir = entity->GetPhysicsPointer()->getPosition()-m_pointOfDetonation;
		real dist=dir.GetMagnitude();
		real st=m_shockWaveSpeed*dt;
		real force =0.0f;
		if((st-(0.5f*m_shockWaveThickness))<=dist&&(dist<st))
		{
			force=1.0f-(st-dist)/(0.5f*m_shockWaveThickness);
			dir.Normalize();
			dir*=force*m_peakConcussiveForce;
			entity->GetPhysicsPointer()->addForce(dir);
		}
		else if((st<=dist)&&(dist<(st+m_shockWaveThickness)))
		{
			force = m_peakConcussiveForce;
			dir.Normalize();
			dir*=force*m_peakConcussiveForce;
			entity->GetPhysicsPointer()->addForce(dir);
		}
		else if(((st+m_shockWaveThickness)<=dist)&& (dist < (st+1.5f*m_shockWaveThickness)))
		{
			force = (dist-st-m_shockWaveThickness)/m_shockWaveThickness*0.5f;
			dir.Normalize();
			dir*=force*m_peakConcussiveForce;
			entity->GetPhysicsPointer()->addForce(dir);
		}
		else
		{
			force = 0.0f;
			dir.Normalize();
			dir*=force*m_peakConcussiveForce;
			entity->GetPhysicsPointer()->addForce(dir);
		}
		return false;
	}
	else if(m_convectionDuration>0.0f)
	{
		m_convectionDuration-=dt;
		DragonXVector3 dir = entity->GetPhysicsPointer()->getPosition()-m_pointOfDetonation;
		real distxz= sqrt(dir.x*dir.x+dir.z*dir.z);
		if((distxz<m_chmineyRadius*2)&&dir.y<m_chimneyHeight)
		{
			real force = m_peakConvectionForce*distxz/m_chmineyRadius;
			dir.Normalize();
			dir*=force;
			entity->GetPhysicsPointer()->addForce(dir);
		}
		return false;
	}
	else
	{
		return true;
	}
}

Thrust::Thrust()
{
	m_durationOfThrust=5.5f;

	// this is drag or the mpg or whatever the magnitude of the thrust will be decreased by this based on dt so
	// the magnitude will be decreased by this amount every second so if mpg you need a new car
	// side note you can make this negative for an increase in thrust till the duration of the thrust force has been reached
	m_decrement=0.5f;

	// this is the magnitude or strength of the thrust
	m_magnitudeOfTrust=5000.0f;

	// this is the direction of the thrust
	// they are stored separately so the magnitude can be decremented
	// and then integrated into the force update by scaling the direction
	m_direction=DragonXVector3(0.0f,1.0f,0.0f);
	m_direction.Normalize();
}
// this is the parameterized constructor
Thrust::Thrust(real duration,real decrement,real magnitude,DragonXVector3 direction)
{
	m_durationOfThrust=duration;
	m_decrement=decrement;
	m_magnitudeOfTrust=magnitude;
	m_direction=direction;
	m_direction.Normalize();
}
Thrust::~Thrust()
{
}
// updates the entity with the force
bool Thrust::updateForce(BaseGameEntity* entity, real dt)
{
	if(!entity->GetPhysicsPointer()->hasFiniteMass())
	{
		return true;
	}
	if(m_durationOfThrust>0.0f)
	{
		m_durationOfThrust-=dt;
		m_magnitudeOfTrust-=m_decrement;
		m_direction.Normalize();
		m_direction*=m_magnitudeOfTrust;
		entity->GetPhysicsPointer()->addForce(m_direction);
		return false;
	}
	else if (m_durationOfThrust==0.0f)
	{
		m_direction.Normalize();
		m_direction*=m_magnitudeOfTrust;
		entity->GetPhysicsPointer()->addForce(m_direction);
		return true;
	}
	else if(m_durationOfThrust==-1.0f)
	{
		m_magnitudeOfTrust-=m_decrement;
		if (m_magnitudeOfTrust>0.0f)
		{
			m_direction.Normalize();
			m_direction*=m_magnitudeOfTrust;
			entity->GetPhysicsPointer()->addForce(m_direction);
			return false;
		}
		else
		{
			return true;
		}
	}
	else
	{
		return true;
	}
}

AnchoredSpringGenerator::AnchoredSpringGenerator()
{
	m_SpringConstant=(real)1.5f;
	m_RestLength=(real)5.0f;
	m_AnchorPosition=DragonXVector3(0.0f,0.0f,0.0f);
	m_OffSet=DragonXVector3(0.0f,0.0f,0.0f);
}

AnchoredSpringGenerator::AnchoredSpringGenerator(real constant, real restLength, DragonXVector3& anchorPos)
{
	m_SpringConstant=constant;
	m_RestLength=restLength;
	m_AnchorPosition=anchorPos;
	m_OffSet=DragonXVector3(0.0f,0.0f,0.0f);
}

AnchoredSpringGenerator::~AnchoredSpringGenerator()
{
}

void AnchoredSpringGenerator::setOffSet(DragonXVector3& offset)
{
	m_OffSet=offset;
}

bool AnchoredSpringGenerator::updateForce(BaseGameEntity* entity, real dt)
{
	DragonXVector3 force;
	force=entity->GetPhysicsPointer()->getPosition();
	force-=m_OffSet;
	force -= m_AnchorPosition;

	real magnitude = force.GetMagnitude();
	magnitude = (magnitude-m_RestLength) * m_SpringConstant;

	force.Normalize();
	force*=-magnitude;
	entity->GetPhysicsPointer()->addForce(force);
	return false;
}

AnchoredBungeeGenerator::AnchoredBungeeGenerator()
{
	m_SpringConstant=(real)1.5f;
	m_RestLength=(real)5.0f;
	m_AnchorPosition=DragonXVector3(0.0f,0.0f,0.0f);
	m_OffSet=DragonXVector3(0.0f,0.0f,0.0f);
}
AnchoredBungeeGenerator::AnchoredBungeeGenerator(real constant, real restLength, DragonXVector3& anchorPos)
{
	m_SpringConstant=constant;
	m_RestLength=restLength;
	m_AnchorPosition=anchorPos;
	m_OffSet=DragonXVector3(0.0f,0.0f,0.0f);
}
AnchoredBungeeGenerator::~AnchoredBungeeGenerator()
{
}
void AnchoredBungeeGenerator::setOffSet(DragonXVector3& offset)
{
	m_OffSet=offset;
}
bool AnchoredBungeeGenerator::updateForce(BaseGameEntity* entity, real dt)
{
	DragonXVector3 force;
	force=entity->GetPhysicsPointer()->getPosition();
	force-=m_OffSet;
	force -= m_AnchorPosition;

	real magnitude = force.GetMagnitude();
	if(magnitude <= m_RestLength)
	{
		return false;
	}
	magnitude = (magnitude-m_RestLength) * m_SpringConstant;

	force.Normalize();
	force*=-magnitude;
	entity->GetPhysicsPointer()->addForce(force);
	return false;
}

ConnectingSpringForceGenerator::ConnectingSpringForceGenerator(real constant, real restLength, BaseGameEntity* otherEntity)
{
	m_SpringConstant=constant;
	m_RestLength=restLength;
	m_OtherEntity=otherEntity;
	m_OffsetForUpdatedEntity=DragonXVector3(0.0f,0.0f,0.0f);
	m_OffsetForOther=DragonXVector3(0.0f,0.0f,0.0f);
}
ConnectingSpringForceGenerator::~ConnectingSpringForceGenerator()
{
}
void ConnectingSpringForceGenerator::setOffSetOther(DragonXVector3& offset)
{
	m_OffsetForUpdatedEntity=offset;
}
void ConnectingSpringForceGenerator::setOffSetMain(DragonXVector3& offset)
{
	m_OffsetForOther=offset;
}
bool ConnectingSpringForceGenerator::updateForce(BaseGameEntity* entity, real dt)
{
	if(m_OtherEntity==nullptr)
	{
		return true;
	}
	else
	{
		DragonXVector3 force;
		force = entity->GetPhysicsPointer()->getPosition()-m_OffsetForUpdatedEntity;
		force -= (m_OtherEntity->GetPhysicsPointer()->getPosition()-m_OffsetForOther);

		real magnitude = force.GetMagnitude();
		magnitude = magnitude - m_RestLength;
		magnitude*=m_SpringConstant;

		force.Normalize();
		force *= -magnitude;
		entity->GetPhysicsPointer()->addForce(force);
		return false;
	}
}

ConnectingBungeeForceGenerator::ConnectingBungeeForceGenerator(real constant, real restLength, BaseGameEntity* otherEntity)
{
	m_SpringConstant=constant;
	m_RestLength=restLength;
	m_OtherEntity=otherEntity;
	m_OffsetForUpdatedEntity=DragonXVector3(0.0f,0.0f,0.0f);
	m_OffsetForOther=DragonXVector3(0.0f,0.0f,0.0f);
}
ConnectingBungeeForceGenerator::~ConnectingBungeeForceGenerator()
{
}
void ConnectingBungeeForceGenerator::setOffSetOther(DragonXVector3& offset)
{
	m_OffsetForUpdatedEntity=offset;
}
void ConnectingBungeeForceGenerator::setOffSetMain(DragonXVector3& offset)
{
	m_OffsetForOther=offset;
}
bool ConnectingBungeeForceGenerator::updateForce(BaseGameEntity* entity, real dt)
{
	if(m_OtherEntity==nullptr)
	{
		return true;
	}
	else
	{
		DragonXVector3 force;
		force = entity->GetPhysicsPointer()->getPosition()-m_OffsetForUpdatedEntity;
		force -= (m_OtherEntity->GetPhysicsPointer()->getPosition()-m_OffsetForOther);

		real magnitude = force.GetMagnitude();
		if(magnitude <= m_RestLength)
		{
			return false;
		}
		magnitude = (magnitude-m_RestLength) * m_SpringConstant;

		force.Normalize();
		force *= -magnitude;
		entity->GetPhysicsPointer()->addForce(force);
		return false;
	}
}