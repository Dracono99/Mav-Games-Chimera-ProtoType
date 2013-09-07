#include "DragonForceRegistry.h"

// registers the given entity and force generator
void DragonForceRegistry::add(BaseGameEntity* entity, DragonForceGenerator* fg)
{
	DragonForceRegistration* r = new DragonForceRegistration;
	r->mEntity=entity;
	r->DFG=fg;
	mDragonRegistrations.push_back(r);
}
// removes given registered pair from list if no pair is found does nothing
void DragonForceRegistry::remove(BaseGameEntity* entity, DragonForceGenerator* fg)
{
	for(auto DragonForceRegistration : mDragonRegistrations)
	{
		if(DragonForceRegistration->mEntity==entity&&DragonForceRegistration->DFG==fg)
		{
			delete DragonForceRegistration;
			DragonForceRegistration=NULL;
		}
	}
	cleanUp();
}
// removes all forces associated with the given entity
void DragonForceRegistry::removeAllFrom(BaseGameEntity* entity)
{
	for(auto DragonForceRegistration : mDragonRegistrations)
	{
		if(DragonForceRegistration->mEntity==entity)
		{
			delete DragonForceRegistration;
			DragonForceRegistration=NULL;
		}
	}
	cleanUp();
}
// removes the given registration from the vector of registrations
void DragonForceRegistry::removeRegistration(DragonForceRegistration* reg)
{
	for(auto DragonForceRegistration : mDragonRegistrations)
	{
		if(DragonForceRegistration==reg)
		{
			delete DragonForceRegistration;
			DragonForceRegistration=NULL;
		}
	}
	cleanUp();
}
// removes a list of registrations from the main registry
void DragonForceRegistry::removeRegistrations(std::vector<DragonForceRegistration*>& regs)
{
	for(const auto DragonForceRegistration : regs)
	{
		removeRegistration(DragonForceRegistration);
	}
	cleanUp();
}
// this will remove all registrations and delete all force generators
void DragonForceRegistry::clear()
{
	for (auto DragonForceRegistration : mDragonRegistrations)
	{
		delete DragonForceRegistration;
		DragonForceRegistration=NULL;
	}
	mDragonRegistrations.clear();
}
// this updates all the registrations and takes in dt
// note this will also automatically delete any forces that need to be deleted
void DragonForceRegistry::updateForces(real dt)
{
	DragonicRegistry regsToKeep;
	for(const auto DragonForceRegistration : mDragonRegistrations)
	{
		if(!DragonForceRegistration->DFG->updateForce(DragonForceRegistration->mEntity,dt))
		{
			regsToKeep.push_back(DragonForceRegistration);
		}
		else
		{
			delete DragonForceRegistration;
		}
	}
	mDragonRegistrations.clear();
	mDragonRegistrations=regsToKeep;
}
void DragonForceRegistry::cleanUp()
{
	DragonicRegistry regsToKeep;
	for(auto DragonForceRegistration : mDragonRegistrations)
	{
		if(DragonForceRegistration)
		{
			regsToKeep.push_back(DragonForceRegistration);
		}
	}
	mDragonRegistrations.clear();
	mDragonRegistrations=regsToKeep;
}