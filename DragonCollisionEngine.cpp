#include "DragonCollisionEngine.h"

// the default constructor just creates a few pointers
DragonCollisionEngine::DragonCollisionEngine()
{
	m_cpContactGenerator = new DragonContactGenerator();
	m_cpContactResolver = new DragonContactResolver();
	m_cHydraSpaceOptimization = new HydraOctree();
}
DragonCollisionEngine::~DragonCollisionEngine()
{
	delete m_cpContactGenerator;
	delete m_cpContactResolver;
	delete m_cHydraSpaceOptimization;
}
void DragonCollisionEngine::GenerateContacts(std::vector<BaseGameEntity*>& entities)
{
	// this will just generate the contacts from the given list of entities the contact generator will store them internally
	// worth noting that as i never use this method it doesn't yet have octrees implimented though it would be easy enough to do so
	m_cHydraSpaceOptimization->Release();
	// this will recursively call the octree creation method basically
	// this is where the entities get split up
	m_cHydraSpaceOptimization->Create(entities,entities.size());
	// the entities are now split up and because the contact generator stores its contacts internally
	// we can just send the contact generator into the octree to perform collision detection on the now sorted entities
	m_cHydraSpaceOptimization->GetContacts(*m_cpContactGenerator,m_cHydraSpaceOptimization);
}
std::vector<DragonContact*> DragonCollisionEngine::GetContactList()
{
	// the contact generator stores contacts internally
	// this is how you get them
	return m_cpContactGenerator->GetContacts();
}
std::vector<DragonContact*> DragonCollisionEngine::GetListOfContactsFromList(std::vector<BaseGameEntity*>& entities)
{
	// first step of generating contacts and performing collision detection
	// clear out our oct trees this will free up memory so that we can
	// re create our octrees as the previously generated ones
	// would have likely been invalidated
	// by the update and resulting
	// change in position of the entities
	m_cHydraSpaceOptimization->Release();
	// this will recursively call the octree creation method basically
	// this is where the entities get split up
	m_cHydraSpaceOptimization->Create(entities,entities.size());
	// the entities are now split up and because the contact generator stores its contacts internally
	// we can just send the contact generator into the octree to perform collision detection on the now sorted entities
	m_cHydraSpaceOptimization->GetContacts(*m_cpContactGenerator,m_cHydraSpaceOptimization);
	// because this method must return the resulting contacts we now return them
	// by getting them from the generator
	return m_cpContactGenerator->registry->m_vContacts;
}
void DragonCollisionEngine::ResolveContacts(real dt)
{
	// the contact resolver uses an iterative resolution process
	// so before we resolve the contacts we decide the number of iterations
	// less iterations is faster
	// more iterations is more accurate
	// i opt for 4 times the number of contacts
	// but it can be reduced to 2 for performance
	m_cpContactGenerator->registry->ResolveContacts(m_cpContactResolver,dt);
	// now that we have set our iterations we resolve the contacts
	// note that this resolves the internal contact list which is currently unused
	// now a down side of the contact generators internal storage of contacts
	// it doesn't know when to delete them or when they
	// have been deleted which btw all contacts are deleted at the end of the resolution stage
	// which means the generator now has a vector full of garbage
	// this will clear the vector which has just garbage in it anyway
	m_cpContactGenerator->ZeroContacts();
}