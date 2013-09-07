#pragma once
#include <vector>
#include "DragonForceGenerator.h"

// a registry to hold all the entities and forces that are to be applied to them

// a struct to hold a pair for registration
struct DragonForceRegistration
{
	BaseGameEntity* mEntity;
	DragonForceGenerator* DFG;
	~DragonForceRegistration()
	{
		delete DFG;
	}
};
// holds a vector of registrations
typedef std::vector<DragonForceRegistration*> DragonicRegistry;

class DragonForceRegistry
{
private:
	//// a struct to hold a pair for registration
	//struct DragonForceRegistration
	//{
	//	BaseGameEntity* mEntity;
	//	DragonForceGenerator* DFG;
	//	~DragonForceRegistration()
	//	{
	//		delete DFG;
	//	}
	//};
	//// holds a vector of registrations
	//typedef std::vector<DragonForceRegistration*> DragonicRegistry;
	DragonicRegistry mDragonRegistrations;
public:
	~DragonForceRegistry()
	{
		clear();
	}
	// registers the given entity and force generator
	void add(BaseGameEntity* entity, DragonForceGenerator* fg);
	// removes given registered pair from list if no pair is found does nothing
	void remove(BaseGameEntity* entity, DragonForceGenerator* fg);
	// removes all forces associated with the given entity
	void removeAllFrom(BaseGameEntity* entity);
	// removes the given registration from the vector of registrations
	void removeRegistration(DragonForceRegistration* reg);
	// removes a list of registrations from the main registry
	void removeRegistrations(std::vector<DragonForceRegistration*>& regs);
	// this will remove all registrations and delete all force generators
	void clear();
	// this updates all the registrations and takes in dt
	// note this will also automatically delete any forces that need to be deleted
	void updateForces(real dt);
	void cleanUp();
};