#include "DragonContactResolver.h"
#include <algorithm>

// Contact resolver implementation
DragonContactResolver::DragonContactResolver()
{
	setIterations(5,5);
	setEpsilon(0.01f, 0.01f);
}
DragonContactResolver::~DragonContactResolver()
{
}

bool DragonContactResolver::isValid()
{
	return (m_iVelocityIterations > 0) &&
		(m_iPositionIterations > 0) &&
		(m_rPositionEpsilon >= 0.01f) &&
		(m_rVelocityEpsilon >= 0.01f);
}
void DragonContactResolver::setIterations(unsigned iterations)
{
	setIterations(iterations, iterations);
}
void DragonContactResolver::setIterations(unsigned velocityIterations, unsigned positionIterations)
{
	m_iVelocityIterations = velocityIterations;
	m_iPositionIterations = positionIterations;
}
void DragonContactResolver::setEpsilon(real velocityEpsilon,real positionEpsilon)
{
	m_rVelocityEpsilon = velocityEpsilon;
	m_rPositionEpsilon = positionEpsilon;
}
void DragonContactResolver::ClearContacts(std::vector<DragonContact*>& contactArray)
{
	for(const auto DragonContact: contactArray)
	{
		delete DragonContact;
	}
	contactArray.clear();
}
void DragonContactResolver::resolveContacts(std::vector<DragonContact*>& contactArray, unsigned numContacts,real duration)
{
	// Make sure we have something to do.
	if (numContacts == 0)
	{
		return;
	}
	if (!isValid())
	{
		return;
	}
	// Prepare the contacts for processing
	prepareContacts(contactArray, numContacts, duration);
	// Resolve the interpenetration problems with the contacts.
	adjustPositions(contactArray, numContacts, duration);
	// Resolve the velocity problems with the contacts.
	adjustVelocities(contactArray, numContacts, duration);
}
void DragonContactResolver::prepareContacts(std::vector<DragonContact*>& contactArray, unsigned numContacts, real duration)
{
	for(const auto DragonContact: contactArray)
	{
		DragonContact->calculateInternals(duration);
	}
}
void DragonContactResolver::adjustVelocities(std::vector<DragonContact*>& c, unsigned numContacts, real duration)
{
	DragonXVector3 velocityChange[2], rotationChange[2];
	DragonXVector3 deltaVel;
	// iteratively handle impacts in order of severity.
	m_iVelocityIterationsUsed = 0;
	while (m_iVelocityIterationsUsed < m_iVelocityIterations)
	{
		// Find contact with maximum magnitude of probable velocity change.
		real max = m_rVelocityEpsilon;
		unsigned index = numContacts;
		/*for (unsigned i = 0; i < numContacts; i++)
		{
		if (c[i]->desiredDeltaVelocity > max)
		{
		max = c[i]->desiredDeltaVelocity;
		index = i;
		}
		}*/
		auto itr = std::max_element(begin(c),end(c),[&](DragonContact* a,DragonContact* b){return a->desiredDeltaVelocity<b->desiredDeltaVelocity;});
		if (itr.operator*()->desiredDeltaVelocity<m_rVelocityEpsilon) break;
		// Match the awake state at the contact
		itr.operator*()->matchAwakeState();
		// Do the resolution on the contact that came out top.
		itr.operator*()->applyVelocityChange(velocityChange, rotationChange);
		// With the change in velocity of the two bodies, the update of
		// contact velocities means that some of the relative closing
		// velocities need recomputing.
		for (unsigned i = 0; i < numContacts; i++)
		{
			// Check each body in the contact
			for (unsigned b = 0; b < 2; b++) if (c[i]->m_aEntitiesContacted[b]->GetPhysicsPointer()->mI_PhysicsType)
			{
				// Check for a match with each body in the newly
				// resolved contact
				for (unsigned d = 0; d < 2; d++)
				{
					if (c[i]->m_aEntitiesContacted[b] == itr.operator*()->m_aEntitiesContacted[d])
					{
						deltaVel = velocityChange[d] + rotationChange[d] % c[i]->relitiveContactPosition[b];
						// The sign of the change is negative if we're dealing
						// with the second body in a contact.
						c[i]->contactVelocity += c[i]->contactToWorld.TransformTranspose(deltaVel)* (real)(b?-1:1);
						c[i]->calculateDesiredDeltaVelocity(duration);
					}
				}
			}
		}
		m_iVelocityIterationsUsed++;
	}
}
void DragonContactResolver::adjustPositions(std::vector<DragonContact*>& c,unsigned numContacts, real duration)
{
	unsigned i,index;
	DragonXVector3 linearChange[2], angularChange[2];
	real max;
	DragonXVector3 deltaPosition;
	// iteratively resolve interpenetration in order of severity.
	m_iPositionIterationsUsed = 0;
	while (m_iPositionIterationsUsed < m_iPositionIterations)
	{
		// Find biggest penetration
		max = m_rPositionEpsilon;
		index = numContacts;
		/*	for (i=0; i<numContacts; i++)
		{
		if (c[i]->m_rPenetration > max)
		{
		max = c[i]->m_rPenetration;
		index = i;
		}
		}*/
		auto itr = std::max_element(begin(c),end(c),[&](DragonContact* a, DragonContact* b){return a->m_rPenetration<b->m_rPenetration;});
		if (itr.operator*()->m_rPenetration<m_rPositionEpsilon) break;
		// Match the awake state at the contact
		itr.operator*()->matchAwakeState();
		// Resolve the penetration.
		itr.operator*()->applyPositionChange(linearChange,angularChange,itr.operator*()->m_rPenetration);
		// Again this action may have changed the penetration of other
		// bodies, so we update contacts.
		for (i = 0; i < numContacts; i++)
		{
			// Check each body in the contact
			for (unsigned b = 0; b < 2; b++) if (c[i]->m_aEntitiesContacted[b]->GetPhysicsPointer()->mI_PhysicsType)
			{
				// Check for a match with each body in the newly
				// resolved contact
				for (unsigned d = 0; d < 2; d++)
				{
					if (c[i]->m_aEntitiesContacted[b] == itr.operator*()->m_aEntitiesContacted[d])
					{
						deltaPosition = linearChange[d] + angularChange[d]%c[i]->relitiveContactPosition[b];
						// The sign of the change is positive if we're
						// dealing with the second body in a contact
						// and negative otherwise (because we're
						// subtracting the resolution)..
						c[i]->m_rPenetration += deltaPosition * c[i]->m_vContactNormal* (b?1:-1);
					}
				}
			}
		}
		m_iPositionIterationsUsed++;
	}
}