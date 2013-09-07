#include "DragonForceEngine.h"

DragonForceEngine::DragonForceEngine()
{
	mDragonForceRegistry=new DragonForceRegistry;
}
DragonForceEngine::~DragonForceEngine()
{
	delete mDragonForceRegistry;
}
void DragonForceEngine::updateForces(real dt)
{
	mDragonForceRegistry->updateForces(dt);
}
//when adding forces try to make them self destroying see the force generators for details
void DragonForceEngine::addThrustToONeEntity(BaseGameEntity* entity, real durationOfThrust, real DecrementOfThrust, real MagnitudeOfThrust, DragonXVector3 &direction)
{
	Thrust* fg =new Thrust(durationOfThrust,DecrementOfThrust,MagnitudeOfThrust,direction);
	mDragonForceRegistry->add(entity,fg);
}
// default force is a temporary force upward like a space rocket
void DragonForceEngine::addDefaultThrustToEntity(BaseGameEntity* entity)
{
	Thrust* fg = new Thrust();
	mDragonForceRegistry->add(entity,fg);
}
//this will add a thrust force to all the entities in this list the same parameters will be used in each thrust force
void DragonForceEngine::addThrustToVectorOfEntities(std::vector<BaseGameEntity*>& entities, real durationOfThrust, real DecrementOfThrust, real MagnitudeOfThrust, DragonXVector3 &direction)
{
	for(const auto BaseGameEntity : entities)
	{
		addThrustToONeEntity(BaseGameEntity,durationOfThrust,DecrementOfThrust,MagnitudeOfThrust,direction);
	}
}
// this does basically the same thing but with default values for the thrust force
void DragonForceEngine::addDeafaultThrustForceToVectorOfEntities(std::vector<BaseGameEntity*>& entities)
{
	for (const auto BaseGameEntity : entities)
	{
		addDefaultThrustToEntity(BaseGameEntity);
	}
}
// this adds gravity towards a point to one entity
// for a black hole just use a real big number for mass
void DragonForceEngine::addGravityFromPointToEntity(BaseGameEntity* entity, DragonXVector3 &positionOfGravity, real massOfGravityObject)
{
	GravityFromPoint* fg = new GravityFromPoint(positionOfGravity,massOfGravityObject);
	mDragonForceRegistry->add(entity,fg);
}
// this adds gravity to a point to a vector of entities
void DragonForceEngine::addGravityFromPointToVectorOfEntites(std::vector<BaseGameEntity*>& entities,DragonXVector3 &positionOfGravityObject, real massOfGravityObject)
{
	for (const auto BaseGameEntity : entities)
	{
		addGravityFromPointToEntity(BaseGameEntity,positionOfGravityObject,massOfGravityObject);
	}
}

// you can thank Nicholson for this one this will apply gravity to both entities based on each entities own mass and their distance from each other
void DragonForceEngine::addGravityBetweenEntities(BaseGameEntity* entity1,BaseGameEntity* entity2)
{
	GravityBetweenEntities* fg = new GravityBetweenEntities(entity1,entity2);
	GravityBetweenEntities* fg2 = new GravityBetweenEntities(entity2,entity1);
	mDragonForceRegistry->add(entity1,fg);
	mDragonForceRegistry->add(entity2,fg2);
}
// and just for insanity this will apply gravity to all entities based on distance and mass will likely have negative effects on performance
void DragonForceEngine::addGravityBetweenAllEntitiesInVector(std::vector<BaseGameEntity*>& entities)
{
	for(int i = 0 ; (unsigned)i < entities.size()-1; i++)
	{
		for (int j=i+1;(unsigned)j < entities.size();j++)
		{
			if(entities[i]->GetPhysicsPointer()->hasFiniteMass()&&entities[j]->GetPhysicsPointer()->hasFiniteMass())
			{
				addGravityBetweenEntities(entities[i],entities[j]);
			}
		}
	}
}
// because all games need big booms this is a default explosion force added to one entity
void DragonForceEngine::addDefaultExplosionToOneEntity(BaseGameEntity* entity)
{
	Explosion* fg = new Explosion();
	mDragonForceRegistry->add(entity,fg);
}
// this will allow for custom explosions as a result its a long one and you will need to enter a value for every parameter but alterations can be made if needed
void DragonForceEngine::addCustomExplosionToOneEntity(BaseGameEntity* entity,DragonXVector3 &pointOfDetonation,real implosionMinRadius,
													  real implosionMaxRadius, real implosionDuration, real implosionForce,real shockWaveSpeed, real shockWaveThickness, real peakConcussiveForce,
													  real concussionDuration, real peakConvectionForce, real chimneyRadius, real chimneyHeight, real convectionDuration )
{
	Explosion* fg = new Explosion();
	fg->m_pointOfDetonation=pointOfDetonation;
	fg->m_implosionMinRadius=implosionMinRadius;
	fg->m_implosionMaxRadius=implosionMaxRadius;
	fg->m_implosionDuration=implosionDuration;
	fg->m_implosionForce=implosionForce;
	fg->m_shockWaveSpeed=shockWaveSpeed;
	fg->m_shockWaveThickness=shockWaveThickness;
	fg->m_peakConcussiveForce=peakConcussiveForce;
	fg->m_concussionDuration=concussionDuration;
	fg->m_peakConvectionForce=peakConvectionForce;
	fg->m_chmineyRadius=chimneyRadius;
	fg->m_chimneyHeight=chimneyHeight;
	fg->m_convectionDuration=convectionDuration;
	mDragonForceRegistry->add(entity,fg);
}
// this is the easiest way of making an explosion
void DragonForceEngine::addDefaultExplosionToVectorOfEntities(std::vector<BaseGameEntity*>& entities)
{
	for (const auto BaseGameEntity : entities)
	{
		addDefaultExplosionToOneEntity(BaseGameEntity);
	}
}
// this is the hard way yes all values do need to be entered as for good values the default ones should be decent
//for a description of the values see the force generator
void DragonForceEngine::addCustomExplosionToVectorOfEntities(std::vector<BaseGameEntity*>& entities,DragonXVector3 &pointOfDetonation,real implosionMinRadius,
															 real implosionMaxRadius, real implosionDuration, real implosionForce,real shockWaveSpeed, real shockWaveThickness, real peakConcussiveForce,
															 real concussionDuration, real peakConvectionForce, real chimneyRadius, real chimneyHeight, real convectionDuration )
{
	for (const auto BaseGameEntity : entities)
	{
		addCustomExplosionToOneEntity(BaseGameEntity,pointOfDetonation,implosionMinRadius,implosionMaxRadius,implosionDuration,
			implosionForce,shockWaveSpeed,shockWaveThickness,peakConcussiveForce,concussionDuration,
			peakConvectionForce,chimneyRadius,chimneyHeight,convectionDuration);
	}
}

void DragonForceEngine::AddDefaultAnchoredSpringForceToEntity(BaseGameEntity* entity)
{
	AnchoredSpringGenerator* fg = new AnchoredSpringGenerator();
	mDragonForceRegistry->add(entity,fg);
}
void DragonForceEngine::AddDefaultAnchoredSpringForceToVectorOfEntities(std::vector<BaseGameEntity*>& entities)
{
	for(const auto BaseGameEntity : entities)
	{
		AddDefaultAnchoredSpringForceToEntity(BaseGameEntity);
	}
}
void DragonForceEngine::AddDefaultAnchoredSpringForceToEntityWithOffSet(BaseGameEntity* entity,DragonXVector3& offset)
{
	AnchoredSpringGenerator* fg = new AnchoredSpringGenerator();
	fg->setOffSet(offset);
	mDragonForceRegistry->add(entity,fg);
}
void DragonForceEngine::AddDefaultAnchoredSpringForceToVectorOfEntitiesWithOffSet(std::vector<BaseGameEntity*>& entities, DragonXVector3& offset)
{
	for(const auto BaseGameEntity : entities)
	{
		AddDefaultAnchoredSpringForceToEntityWithOffSet(BaseGameEntity,offset);
	}
}
void DragonForceEngine::AddCustomAnchoredSpringForceToEntity(BaseGameEntity* entity,real springConstant, real restLength, DragonXVector3& anchorPos)
{
	AnchoredSpringGenerator* fg =new AnchoredSpringGenerator(springConstant,restLength,anchorPos);
	mDragonForceRegistry->add(entity,fg);
}
void DragonForceEngine::AddCustomAnchoredSpringForceToVectorOfEntities(std::vector<BaseGameEntity*>& entities, real springConstant, real restLength, DragonXVector3& anchorPos)
{
	for(const auto BaseGameEntity : entities)
	{
		AddCustomAnchoredSpringForceToEntity(BaseGameEntity,springConstant,restLength,anchorPos);
	}
}
void DragonForceEngine::AddCustomAnchoredSpringForceToEntityWithOffSet(BaseGameEntity* entity,real springConstant, real restLength,DragonXVector3& anchorPos,DragonXVector3& offset)
{
	AnchoredSpringGenerator* fg =new AnchoredSpringGenerator(springConstant,restLength,anchorPos);
	fg->setOffSet(offset);
	mDragonForceRegistry->add(entity,fg);
}
void DragonForceEngine::AddCustomAnchoredSpringForceToVectorOfEntitiesWithOffSet(std::vector<BaseGameEntity*>& entities, real springConstant, real restLength, DragonXVector3& anchorPos, DragonXVector3& offset)
{
	for (const auto BaseGameEntity : entities)
	{
		AddCustomAnchoredSpringForceToEntityWithOffSet(BaseGameEntity,springConstant,restLength,anchorPos,offset);
	}
}
void DragonForceEngine::AddDefaultAnchoredBungeeForceToEntity(BaseGameEntity* entity)
{
	AnchoredBungeeGenerator* fg=new AnchoredBungeeGenerator();
	mDragonForceRegistry->add(entity,fg);
}
void DragonForceEngine::AddDefaultAnchoredBungeeForceToVectorOfEntities(std::vector<BaseGameEntity*>& entities)
{
	for (const auto BaseGameEntity : entities)
	{
		AddDefaultAnchoredBungeeForceToEntity(BaseGameEntity);
	}
}
void DragonForceEngine::AddDefaultAnchoredBungeeForceToEntityWithOffSet(BaseGameEntity* entity,DragonXVector3& offset)
{
	AnchoredBungeeGenerator* fg=new AnchoredBungeeGenerator();
	fg->setOffSet(offset);
	mDragonForceRegistry->add(entity,fg);
}
void DragonForceEngine::AddDefaultAnchoredBungeeForceToVectorOfEntitiesWithOffSet(std::vector<BaseGameEntity*>& entities, DragonXVector3& offset)
{
	for (const auto BaseGameEntity : entities)
	{
		AddDefaultAnchoredBungeeForceToEntityWithOffSet(BaseGameEntity,offset);
	}
}
void DragonForceEngine::AddCustomAnchoredBungeeForceToEntity(BaseGameEntity* entity,real springConstant, real restLength, DragonXVector3& anchorPos)
{
	AnchoredBungeeGenerator* fg=new AnchoredBungeeGenerator(springConstant,restLength,anchorPos);
	mDragonForceRegistry->add(entity,fg);
}
void DragonForceEngine::AddCustomAnchoredBungeeForceToVectorOfEntities(std::vector<BaseGameEntity*>& entities, real springConstant, real restLength, DragonXVector3& anchorPos)
{
	for (const auto BaseGameEntity : entities)
	{
		AddCustomAnchoredBungeeForceToEntity(BaseGameEntity,springConstant,restLength,anchorPos);
	}
}
void DragonForceEngine::AddCustomAnchoredBungeeForceToEntityWithOffSet(BaseGameEntity* entity,real springConstant, real restLength,DragonXVector3& anchorPos,DragonXVector3& offset)
{
	AnchoredBungeeGenerator* fg=new AnchoredBungeeGenerator(springConstant,restLength,anchorPos);
	fg->setOffSet(offset);
	mDragonForceRegistry->add(entity,fg);
}
void DragonForceEngine::AddCustomAnchoredBungeeForceToVectorOfEntitiesWithOffSet(std::vector<BaseGameEntity*>& entities, real springConstant, real restLength, DragonXVector3& anchorPos, DragonXVector3& offset)
{
	for (const auto BaseGameEntity : entities)
	{
		AddCustomAnchoredBungeeForceToEntityWithOffSet(BaseGameEntity,springConstant,restLength,anchorPos,offset);
	}
}
void DragonForceEngine::AddSpringForceToAnchorOneEntityToAnother(BaseGameEntity* entity, real restLength, real springConstant, BaseGameEntity* AnchorEntity)
{
	ConnectingSpringForceGenerator* fg =new ConnectingSpringForceGenerator(springConstant,restLength,AnchorEntity);
	mDragonForceRegistry->add(entity,fg);
}
void DragonForceEngine::AddSpringForceToAnchorMultipleEntitiesToOneEntity(std::vector<BaseGameEntity*>& entities, real restLength, real springConstant, BaseGameEntity* AnchorEntity)
{
	for (const auto BaseGameEntity : entities)
	{
		AddSpringForceToAnchorOneEntityToAnother(BaseGameEntity,restLength,springConstant,AnchorEntity);
	}
}
void DragonForceEngine::AddSpringForceToAnchorOneEntityToAnotherWithOffsets(BaseGameEntity* entity, real restLength, real springConstant, BaseGameEntity* AnchorEntity, DragonXVector3& offSetForEntity, DragonXVector3& offsetForAnchor)
{
	ConnectingSpringForceGenerator* fg =new ConnectingSpringForceGenerator(springConstant,restLength,AnchorEntity);
	fg->setOffSetMain(offSetForEntity);
	fg->setOffSetOther(offsetForAnchor);
	mDragonForceRegistry->add(entity,fg);
}
void DragonForceEngine::AddSpringForceToAnchorMultipleEntitiesToOneEntityWithOffsets(std::vector<BaseGameEntity*>& entities, real restLength, real springConstant, BaseGameEntity* AnchorEntity, DragonXVector3& offsetForEntities, DragonXVector3& offsetForAnchor)
{
	for (const auto BaseGameEntity : entities)
	{
		AddSpringForceToAnchorOneEntityToAnotherWithOffsets(BaseGameEntity,restLength,springConstant,AnchorEntity,offsetForEntities,offsetForAnchor);
	}
}
void DragonForceEngine::AddBungeeForceToAnchorOneEntityToAnother(BaseGameEntity* entity, real restLength, real springConstant, BaseGameEntity* AnchorEntity)
{
	ConnectingBungeeForceGenerator* fg =new ConnectingBungeeForceGenerator(springConstant,restLength,AnchorEntity);
	mDragonForceRegistry->add(entity,fg);
}
void DragonForceEngine::AddBungeeForceToAnchorMultipleEntitiesToOneEntity(std::vector<BaseGameEntity*>& entities, real restLength, real springConstant, BaseGameEntity* AnchorEntity)
{
	for (const auto BaseGameEntity : entities)
	{
		AddBungeeForceToAnchorOneEntityToAnother(BaseGameEntity,restLength,springConstant,AnchorEntity);
	}
}
void DragonForceEngine::AddBungeeForceToAnchorOneEntityToAnotherWithOffsets(BaseGameEntity* entity, real restLength, real springConstant, BaseGameEntity* AnchorEntity, DragonXVector3& offSetForEntity, DragonXVector3& offsetForAnchor)
{
	ConnectingBungeeForceGenerator* fg =new ConnectingBungeeForceGenerator(springConstant,restLength,AnchorEntity);
	fg->setOffSetMain(offSetForEntity);
	fg->setOffSetOther(offsetForAnchor);
	mDragonForceRegistry->add(entity,fg);
}
void DragonForceEngine::AddBungeeForceToAnchorMultipleEntitiesToOneEntityWithOffsets(std::vector<BaseGameEntity*>& entities, real restLength, real springConstant, BaseGameEntity* AnchorEntity, DragonXVector3& offsetForEntities, DragonXVector3& offsetForAnchor)
{
	for (const auto BaseGameEntity : entities)
	{
		AddBungeeForceToAnchorOneEntityToAnotherWithOffsets(BaseGameEntity,restLength,springConstant,AnchorEntity,offsetForEntities,offsetForAnchor);
	}
}
void DragonForceEngine::AddSpringForceForConnectingTwoEntitiesTogether(BaseGameEntity* entity1,BaseGameEntity* entity2,real restLength,real springConstant)
{
	ConnectingSpringForceGenerator* fg1 = new ConnectingSpringForceGenerator(springConstant,restLength,entity2);
	ConnectingSpringForceGenerator* fg2 =new ConnectingSpringForceGenerator(springConstant,restLength,entity1);
	mDragonForceRegistry->add(entity1,fg1);
	mDragonForceRegistry->add(entity2,fg2);
}
void DragonForceEngine::AddSpringForceForConnectingTwoEntitiesTogetherWithOffsets(BaseGameEntity* entity1,BaseGameEntity* entity2, real restLength, real springConstant, DragonXVector3& offsetForEntity1,DragonXVector3& offsetForEntity2)
{
	ConnectingSpringForceGenerator* fg1 = new ConnectingSpringForceGenerator(springConstant,restLength,entity2);
	ConnectingSpringForceGenerator* fg2 =new ConnectingSpringForceGenerator(springConstant,restLength,entity1);
	fg1->setOffSetMain(offsetForEntity1);
	fg1->setOffSetOther(offsetForEntity2);
	fg2->setOffSetMain(offsetForEntity2);
	fg2->setOffSetOther(offsetForEntity1);
	mDragonForceRegistry->add(entity1,fg1);
	mDragonForceRegistry->add(entity2,fg2);
}
void DragonForceEngine::AddBungeeForceForConnectingTwoEntitiesTogether(BaseGameEntity* entity1,BaseGameEntity* entity2,real restLength,real springConstant)
{
	ConnectingBungeeForceGenerator* fg1 = new ConnectingBungeeForceGenerator(springConstant,restLength,entity2);
	ConnectingBungeeForceGenerator* fg2 =new ConnectingBungeeForceGenerator(springConstant,restLength,entity1);
	mDragonForceRegistry->add(entity1,fg1);
	mDragonForceRegistry->add(entity2,fg2);
}
void DragonForceEngine::AddBungeeForceForConnectingTwoEntitiesTogetherWithOffsets(BaseGameEntity* entity1,BaseGameEntity* entity2, real restLength, real springConstant, DragonXVector3& offsetForEntity1,DragonXVector3& offsetForEntity2)
{
	ConnectingBungeeForceGenerator* fg1 = new ConnectingBungeeForceGenerator(springConstant,restLength,entity2);
	ConnectingBungeeForceGenerator* fg2 =new ConnectingBungeeForceGenerator(springConstant,restLength,entity1);
	fg1->setOffSetMain(offsetForEntity1);
	fg1->setOffSetOther(offsetForEntity2);
	fg2->setOffSetMain(offsetForEntity2);
	fg2->setOffSetOther(offsetForEntity1);
	mDragonForceRegistry->add(entity1,fg1);
	mDragonForceRegistry->add(entity2,fg2);
}