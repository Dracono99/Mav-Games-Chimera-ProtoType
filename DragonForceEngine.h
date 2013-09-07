#pragma once
#include "DragonForceRegistry.h"

class DragonForceEngine
{
private:
	DragonForceRegistry* mDragonForceRegistry;
public:
	DragonForceEngine();
	~DragonForceEngine();
	void updateForces(real dt);
	//when adding forces try to make them self destroying see the force generators for details
	void addThrustToONeEntity(BaseGameEntity* entity, real durationOfThrust, real DecrementOfThrust, real MagnitudeOfThrust, DragonXVector3 &direction);
	void addDefaultThrustToEntity(BaseGameEntity* entity);// default force is a temporary force upward like a space rocket
	//this will add a thrust force to all the entities in this list the same parameters will be used in each thrust force
	void addThrustToVectorOfEntities(std::vector<BaseGameEntity*>& entities, real durationOfThrust, real DecrementOfThrust, real MagnitudeOfThrust, DragonXVector3 &direction);
	// this does basically the same thing but with default values for the thrust force
	void addDeafaultThrustForceToVectorOfEntities(std::vector<BaseGameEntity*>& entities);
	// this adds gravity towards a point to one entity
	void addGravityFromPointToEntity(BaseGameEntity* entity, DragonXVector3 &positionOfGravity, real massOfGravityObject);// for a black hole just use a real big number for mass
	// this adds gravity to a point to a vector of entities
	void addGravityFromPointToVectorOfEntites(std::vector<BaseGameEntity*>& entities,DragonXVector3 &positionOfGravityObject, real massOfGravityObject);// this will be used to demo straight gravity and gravity to a point
	// you can thank Nicholson for this one this will apply gravity to both entities based on each entities own mass and their distance from each other
	void addGravityBetweenEntities(BaseGameEntity* entity1,BaseGameEntity* entity2);
	// and just for insanity this will apply gravity to all entities based on distance and mass will likely have negative effects on performance
	void addGravityBetweenAllEntitiesInVector(std::vector<BaseGameEntity*>& entities);
	// because all games need big booms this is a default explosion force added to one entity
	void addDefaultExplosionToOneEntity(BaseGameEntity* entity);
	// this will allow for custom explosions as a result its a long one and you will need to enter a value for every parameter but alterations can be made if needed
	void addCustomExplosionToOneEntity(BaseGameEntity* entity,DragonXVector3 &pointOfDetonation,real implosionMinRadius,
		real implosionMaxRadius, real implosionDuration, real implosionForce,real shockWaveSpeed, real shockWaveThickness, real peakConcussiveForce,
		real concussionDuration, real peakConvectionForce, real chimneyRadius, real chimneyHeight, real convectionDuration );
	// this is the easiest way of making an explosion
	void addDefaultExplosionToVectorOfEntities(std::vector<BaseGameEntity*>& entities);
	// this is the hard way yes all values do need to be entered as for good values the default ones should be decent
	//for a description of the values see the force generator
	void addCustomExplosionToVectorOfEntities(std::vector<BaseGameEntity*>& entities,DragonXVector3 &pointOfDetonation,real implosionMinRadius,
		real implosionMaxRadius, real implosionDuration, real implosionForce,real shockWaveSpeed, real shockWaveThickness, real peakConcussiveForce,
		real concussionDuration, real peakConvectionForce, real chimneyRadius, real chimneyHeight, real convectionDuration );
	void AddDefaultAnchoredSpringForceToEntity(BaseGameEntity* entity);
	void AddDefaultAnchoredSpringForceToVectorOfEntities(std::vector<BaseGameEntity*>& entities);
	void AddDefaultAnchoredSpringForceToEntityWithOffSet(BaseGameEntity* entity,DragonXVector3& offset);
	void AddDefaultAnchoredSpringForceToVectorOfEntitiesWithOffSet(std::vector<BaseGameEntity*>& entities, DragonXVector3& offset);
	void AddCustomAnchoredSpringForceToEntity(BaseGameEntity* entity,real springConstant, real restLength, DragonXVector3& anchorPos);
	void AddCustomAnchoredSpringForceToVectorOfEntities(std::vector<BaseGameEntity*>& entities, real springConstant, real restLength, DragonXVector3& anchorPos);
	void AddCustomAnchoredSpringForceToEntityWithOffSet(BaseGameEntity* entity,real springConstant, real restLength,DragonXVector3& anchorPos,DragonXVector3& offset);
	void AddCustomAnchoredSpringForceToVectorOfEntitiesWithOffSet(std::vector<BaseGameEntity*>& entities, real springConstant, real restLength, DragonXVector3& anchorPos, DragonXVector3& offset);
	void AddDefaultAnchoredBungeeForceToEntity(BaseGameEntity* entity);
	void AddDefaultAnchoredBungeeForceToVectorOfEntities(std::vector<BaseGameEntity*>& entities);
	void AddDefaultAnchoredBungeeForceToEntityWithOffSet(BaseGameEntity* entity,DragonXVector3& offset);
	void AddDefaultAnchoredBungeeForceToVectorOfEntitiesWithOffSet(std::vector<BaseGameEntity*>& entities, DragonXVector3& offset);
	void AddCustomAnchoredBungeeForceToEntity(BaseGameEntity* entity,real springConstant, real restLength, DragonXVector3& anchorPos);
	void AddCustomAnchoredBungeeForceToVectorOfEntities(std::vector<BaseGameEntity*>& entities, real springConstant, real restLength, DragonXVector3& anchorPos);
	void AddCustomAnchoredBungeeForceToEntityWithOffSet(BaseGameEntity* entity,real springConstant, real restLength,DragonXVector3& anchorPos,DragonXVector3& offset);
	void AddCustomAnchoredBungeeForceToVectorOfEntitiesWithOffSet(std::vector<BaseGameEntity*>& entities, real springConstant, real restLength, DragonXVector3& anchorPos, DragonXVector3& offset);
	void AddSpringForceToAnchorOneEntityToAnother(BaseGameEntity* entity, real restLength, real springConstant, BaseGameEntity* AnchorEntity);
	void AddSpringForceToAnchorMultipleEntitiesToOneEntity(std::vector<BaseGameEntity*>& entities, real restLength, real springConstant, BaseGameEntity* AnchorEntity);
	void AddSpringForceToAnchorOneEntityToAnotherWithOffsets(BaseGameEntity* entity, real restLength, real springConstant, BaseGameEntity* AnchorEntity, DragonXVector3& offSetForEntity, DragonXVector3& offsetForAnchor);
	void AddSpringForceToAnchorMultipleEntitiesToOneEntityWithOffsets(std::vector<BaseGameEntity*>& entities, real restLength, real springConstant, BaseGameEntity* AnchorEntity, DragonXVector3& offsetForEntities, DragonXVector3& offsetForAnchor);
	void AddBungeeForceToAnchorOneEntityToAnother(BaseGameEntity* entity, real restLength, real springConstant, BaseGameEntity* AnchorEntity);
	void AddBungeeForceToAnchorMultipleEntitiesToOneEntity(std::vector<BaseGameEntity*>& entities, real restLength, real springConstant, BaseGameEntity* AnchorEntity);
	void AddBungeeForceToAnchorOneEntityToAnotherWithOffsets(BaseGameEntity* entity, real restLength, real springConstant, BaseGameEntity* AnchorEntity, DragonXVector3& offSetForEntity, DragonXVector3& offsetForAnchor);
	void AddBungeeForceToAnchorMultipleEntitiesToOneEntityWithOffsets(std::vector<BaseGameEntity*>& entities, real restLength, real springConstant, BaseGameEntity* AnchorEntity, DragonXVector3& offsetForEntities, DragonXVector3& offsetForAnchor);
	void AddSpringForceForConnectingTwoEntitiesTogether(BaseGameEntity* entity1,BaseGameEntity* entity2,real restLength,real springConstant);
	void AddSpringForceForConnectingTwoEntitiesTogetherWithOffsets(BaseGameEntity* entity1,BaseGameEntity* entity2, real restLength, real springConstant, DragonXVector3& offsetForEntity1,DragonXVector3& offsetForEntity2);
	void AddBungeeForceForConnectingTwoEntitiesTogether(BaseGameEntity* entity1,BaseGameEntity* entity2,real restLength,real springConstant);
	void AddBungeeForceForConnectingTwoEntitiesTogetherWithOffsets(BaseGameEntity* entity1,BaseGameEntity* entity2, real restLength, real springConstant, DragonXVector3& offsetForEntity1,DragonXVector3& offsetForEntity2);
};