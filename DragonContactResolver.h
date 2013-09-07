#pragma once

#include "DragonContact.h"
#include <vector>

class DragonContactResolver
{
private:
	//number of iterations to perform when resolving velocity
	unsigned m_iVelocityIterations;
	// number of iterations to perform when resolving position
	unsigned m_iPositionIterations;
	// to reduce instability velocities less than this are considered 0 good starting point is 0.01
	real m_rVelocityEpsilon;
	// to avoid instability penetrations smaller than this are considered zero good starting point is 0.01.
	real m_rPositionEpsilon;
	// keeps track of weather the internal values are valid basically the above variables being kept in check
	bool m_bValidSettings;
	/**
	* Sets up contacts ready for processing. This makes sure their
	* internal data is configured correctly and the correct set of bodies
	* is made alive.
	*/
	void prepareContacts(std::vector<DragonContact*>& contactArray,unsigned numContacts,real duration);
	/**
	* Resolves the velocity issues with the given array of constraints,
	* using the given number of iterations.
	*/
	void adjustVelocities(std::vector<DragonContact*>& contactArray,unsigned numContacts,real duration);
	/**
	* Resolves the positional issues with the given array of constraints,
	* using the given number of iterations.
	*/
	void adjustPositions(std::vector<DragonContact*>& contactArray,unsigned numContacts,real duration);

public:
	//default constructor sets the internal values like epsilon and iterations automatically to predetermined values
	DragonContactResolver();
	// destructor theres not much to say the resolver doesn't really have anything important in it well pointers wise
	~DragonContactResolver();
	// stores the number of velocity iterations used in the last resolve call
	unsigned m_iVelocityIterationsUsed;
	//ditto for position or penetration
	unsigned m_iPositionIterationsUsed;
	// returns true if the resolver has valid settings and is ready to go
	bool isValid();
	/**
	* Sets the number of iterations for each resolution stage.
	*/
	void setIterations(unsigned velocityIterations,unsigned positionIterations);
	/**
	* Sets the number of iterations for both resolution stages.
	*/
	void setIterations(unsigned iterations);
	/**
	* Sets the tolerance value for both velocity and position.
	*/
	void setEpsilon(real velocityEpsilon,real positionEpsilon);
	// the magical dragon contact resolver the main call made to this object give it entities and it does the rest oh umm it will need the size of the vector and the dt so that it can calculate the forces properly
	void resolveContacts( std::vector<DragonContact*>& contactArray,unsigned numContacts,real duration);
	// this is used at the end of the contact resolver after it has resolved all its contacts for this frame it will delete them and clear the vector this is to prevent memory leaks
	void ClearContacts(std::vector<DragonContact*>& contactArray);
	real GetEpsilon()
	{
		return (m_rVelocityEpsilon+m_rPositionEpsilon)/(real)2.0f;
	}
};
