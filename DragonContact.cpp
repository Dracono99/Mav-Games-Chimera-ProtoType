#include "DragonContact.h"
#include "BasePhysicsEntity.h"
#include <cassert>
#include "DragonEnums.h"

DragonContact::DragonContact()
{
	// in case we decide to add friction ill leave an identity matrix here
	DragonXMatrix m;
	D3DXMatrixIdentity(&m);
	contactToWorld=m;
	m_rFriction=9.5f;
}
DragonContact::~DragonContact()// nothing to delete the only pointers are to entities which will be deleted on their own
{
}
// this will wake up a sleeping object if one of the 2 objects are asleep
void DragonContact::matchAwakeState()
{
	// Collisions with the world never cause an entity to wake up
	if (m_aEntitiesContacted[1]->GetPhysicsPointer()->mI_PhysicsType==COT_World) return;
	bool m_aEntitiesContacted0awake = m_aEntitiesContacted[0]->GetPhysicsPointer()->getAwake();
	bool m_aEntitiesContacted1awake = m_aEntitiesContacted[1]->GetPhysicsPointer()->getAwake();
	// Wake up only the sleeping one
	if (m_aEntitiesContacted0awake ^ m_aEntitiesContacted1awake)
	{
		if (m_aEntitiesContacted0awake)
		{
			m_aEntitiesContacted[1]->GetPhysicsPointer()->setAwake(true);
		}
		else
		{
			m_aEntitiesContacted[0]->GetPhysicsPointer()->setAwake(true);
		}
	}
}
/*
* Swaps the bodies in the current DragonContact, so m_aEntitiesContacted 0 is at m_aEntitiesContacted 1 and
* vice versa. This also changes the direction of the DragonContact normal,
* but doesn't update any calculated internal data. If you are calling
* this method manually, then call calculateInternals afterwards to
* make sure the internal data is up to date.
*/
void DragonContact::swapBodies()
{
	m_vContactNormal.invert();
	BaseGameEntity *temp = m_aEntitiesContacted[0];
	m_aEntitiesContacted[0] = m_aEntitiesContacted[1];
	m_aEntitiesContacted[1] = temp;
}

// this sets the contact data allowing the use of the default constructor making contact creation easier
void DragonContact::SetContactData(BaseGameEntity* entity1,BaseGameEntity* entity2,real restituion)
{
	m_aEntitiesContacted[0]=entity1;
	m_aEntitiesContacted[1]=entity2;
	m_rRestitution=restituion;
}
/*
* Constructs an arbitrary orthonormal basis for the DragonContact. This is
* stored as a 3x3 matrix, where each vector is a column (in other
* words the matrix transforms DragonContact space into world space). The x
* direction is generated from the DragonContact normal, and the y and z
* directions are set so they are at right angles to it.
*/
inline void DragonContact::calculateContactBasis()
{
	// this is primarily used for friction in the case of our frictionless envirnment it has limited uses mostly used in
	// the contact resolver during its iteration s to update velosities between resolutions
	DragonXVector3 DragonContactTangent[2];
	// Check whether the Z-axis is nearer to the X or Y axis
	if ((real)abs(m_vContactNormal.x) > (real)abs(m_vContactNormal.y))
	{
		// Scaling factor to ensure the results are normalized
		const real s = (real)1.0f/(real)sqrt(m_vContactNormal.z*m_vContactNormal.z +
			m_vContactNormal.x*m_vContactNormal.x);
		// The new X-axis is at right angles to the world Y-axis
		DragonContactTangent[0].x=m_vContactNormal.z*s;
		DragonContactTangent[0].y= 0.0f;
		DragonContactTangent[0].z= -m_vContactNormal.x*s;
		// The new Y-axis is at right angles to the new X- and Z- axes
		DragonContactTangent[1].x= m_vContactNormal.y*DragonContactTangent[0].x;
		DragonContactTangent[1].y=m_vContactNormal.z*DragonContactTangent[0].x -
			m_vContactNormal.x*DragonContactTangent[0].z;
		DragonContactTangent[1].z=-m_vContactNormal.y*DragonContactTangent[0].x;
		//contactToWorld.setComponents(m_vContactNormal,DragonContactTangent[1],DragonContactTangent[0]);
	}
	else
	{
		// Scaling factor to ensure the results are normalized
		const real s = (real)1.0/(real)sqrt(m_vContactNormal.z*m_vContactNormal.z +
			m_vContactNormal.y*m_vContactNormal.y);
		// The new X-axis is at right angles to the world X-axis
		DragonContactTangent[0].x=(0);
		DragonContactTangent[0].y=-m_vContactNormal.z*s;
		DragonContactTangent[0].z=m_vContactNormal.y*s;

		// The new Y-axis is at right angles to the new X- and Z- axes
		DragonContactTangent[1].x= m_vContactNormal.y*DragonContactTangent[0].z -
			m_vContactNormal.z*DragonContactTangent[0].y;
		DragonContactTangent[1].y=-m_vContactNormal.x*DragonContactTangent[0].z;
		DragonContactTangent[1].z=m_vContactNormal.x*DragonContactTangent[0].y;
	}
	// Make a matrix from the three vectors.
	//this will create the orthanormal basis matrix as a 4 x 4 with the three orthoganal vectors taking up the
	// first 3 rows n columns and spot 44is a 1 because of the identity matrix created in the constructor
	contactToWorld.setComponents(m_vContactNormal,DragonContactTangent[0],DragonContactTangent[1]);
}
DragonXVector3 DragonContact::calculateLocalVelocity(unsigned entityIndex, real dt)
{
	// just for ease of use create a pointer to the entity we wanna use
	BaseGameEntity *thisEntitiesContacted = m_aEntitiesContacted[entityIndex];
	// Work out the velocity of the DragonContact point.
	// this will determine the velocity of the point
	// this will be used in the impulse calculation
	// note we need our rotational velocity in world space so we apply transform direction
	DragonXVector3 velocity = thisEntitiesContacted->GetPhysicsPointer()->TransformMatrix.TransformDirection(thisEntitiesContacted->GetPhysicsPointer()->getRotation())%relitiveContactPosition[entityIndex];

	// above will only get us the rotational velocity at the point
	// for impulse to work we need the velocity of the center of mass as well
	// which for us will just be the velocity of the object
	velocity += thisEntitiesContacted->GetPhysicsPointer()->getVelocity();

	// now for our friction and resting contacts to work properly
	// we need to move our velocity into contact coordinates
	// using our contact to world orthonormal basis
	DragonXVector3 contactVel = contactToWorld.TransformTranspose(velocity);

	// ok now lets account for the acceleration from forces
	DragonXVector3 accVel = thisEntitiesContacted->GetPhysicsPointer()->getLastFrameAcceleration() * dt;

	//and again move to contact coordinates
	accVel = contactToWorld.TransformTranspose(accVel);

	// we are only interested in the accel in the planar directions so not the contact normal direction
	// so we set the x component of the accel to 0
	accVel.x=0.0f;

	// combine the planar accel with contact velocity
	// if theres enough friction they will be removed
	contactVel+=accVel;

	// we have what we cam for return it
	//velocity=contactToWorld.TransformTranspose(velocity);
	return contactVel;
}
void DragonContact::calculateDesiredDeltaVelocity(real dt)
{
	const static real velocityLimit = (real).5f;

	// stacking fix remove excess velocity induced by forces
	real velFromAcc = 0.0f;

	// calclate acceleration induced velocity
	if(m_aEntitiesContacted[0]->GetPhysicsPointer()->getAwake())
	{
		velFromAcc+=m_aEntitiesContacted[0]->GetPhysicsPointer()->getLastFrameAcceleration() * dt * m_vContactNormal;
	}

	// check if body 2 is a game object if not is it awake
	if(m_aEntitiesContacted[1]->GetPhysicsPointer()->mI_PhysicsType&&m_aEntitiesContacted[1]->GetPhysicsPointer()->getAwake())
	{
		velFromAcc-=m_aEntitiesContacted[1]->GetPhysicsPointer()->getLastFrameAcceleration() * dt * m_vContactNormal;
	}

	// If the velocity is very slow, limit the restitution
	// they basically stopped moving so lets just let em settle down
	real rest = m_rRestitution;
	if ((real)abs(contactVelocity.GetMagSquared()) < velocityLimit)
	{
		rest = (real)0.0f;
	}
	// impulse only occurs in the direction of the contact normal we also want to remove any excess velocity from acceleration due to forces
	// so we remove it the use the restitution
	desiredDeltaVelocity= -contactVelocity.x - rest * (contactVelocity.x - velFromAcc);
}
void DragonContact::calculateInternals(real dt)
{
	// this will calculate some internal values for use in the contact resolution
	// Check if the first object is NULL, and swap if it is.
	if (!m_aEntitiesContacted[0]->GetPhysicsPointer()->mI_PhysicsType)
	{
		// this will swap object 1 with object 2
		// this is helpful in that if object 1 is a world object it will be moved to object 2
		// and then checks will keep the world object
		// now at place 2 from being used or updated in the resolution
		swapBodies();
	}
	assert(m_aEntitiesContacted[0]->GetPhysicsPointer()->mI_PhysicsType&&"We have a contact generated between two world objects this should not be possible.");

	// this will create the orhanormal basis which can be used for friction
	calculateContactBasis();
	// Store the relative position of the DragonContact relative to each entity
	// this is done by subrtracting the objects position from the contact position
	relitiveContactPosition[0] = m_vContactPoint - m_aEntitiesContacted[0]->GetPhysicsPointer()->getPosition();
	if (m_aEntitiesContacted[1]->GetPhysicsPointer()->mI_PhysicsType)
	{
		// the first of many checks to see if obj2 is a world body if its not then lets get its relitive contact position too
		relitiveContactPosition[1] = m_vContactPoint - m_aEntitiesContacted[1]->GetPhysicsPointer()->getPosition();
	}
	// Find the relative velocity of the bodies at the DragonContact point.
	// this calculates the closing velocity for each body at the point of contact
	contactVelocity = calculateLocalVelocity(0, dt);
	if (m_aEntitiesContacted[1]->GetPhysicsPointer()->mI_PhysicsType)
	{
		// and then subjtracts one from the other so we have a change in velocity
		contactVelocity -= calculateLocalVelocity(1, dt);
	}
	// and finally calcute expected change in velocity
	calculateDesiredDeltaVelocity(dt);
}
void DragonContact::applyVelocityChange(DragonXVector3 velocityChange[2],DragonXVector3 rotationChange[2])
{
	//chache a copy of each entities invese inertia tensor in world space
	DragonXMatrix inverseInertiaTensor[2];
	inverseInertiaTensor[0]=m_aEntitiesContacted[0]->GetPhysicsPointer()->getInverseInertiaTensorWorld();
	if (m_aEntitiesContacted[1]->GetPhysicsPointer()->mI_PhysicsType)
	{
		// if the second body is not a world entity
		// note i use body and entity interchangeably body is just so much easier to type
		inverseInertiaTensor[1]=m_aEntitiesContacted[1]->GetPhysicsPointer()->getInverseInertiaTensorWorld();
	}
	// heres is just a local vector 3 we'll use him in a bit
	DragonXVector3 impulseDragonContact;
	// we are in space so we will be using a frictionless environment
	//impulseDragonContact = calculateFrictionlessImpulse(inverseInertiaTensor);
	impulseDragonContact = calculateFrictionImpulse(inverseInertiaTensor);
	// impulse is a scalar so obtain it as such
	//real impulse = impulseDragonContact.x;
	DragonXVector3 impulse = contactToWorld.TransformDirection(impulseDragonContact);
	// Split in the impulse into linear and rotational components
	// here we calculate the impulsive torque which is the part of the impulse that will be used for rotational velocity
	//DragonXVector3 impulsiveTorque = relitiveContactPosition[0]%(impulse*m_vContactNormal);
	DragonXVector3 impulsiveTorque = relitiveContactPosition[0]%impulse;
	// the rotational velocity will need to be transformed by the inverse inertia tensor
	// note i use transform direction as the inertia tensor is a rotation matrix essentially
	// a 3x3 being stored in a 4x4 directx matrix so to avoid numerical issues such as negitive zeros
	// i apply a transform that only uses the directional or rotational part of the matrix also has a few less calculations
	rotationChange[0] = inverseInertiaTensor[0].TransformDirection(impulsiveTorque);
	// just to be sure lets zero this one out
	velocityChange[0].clear();
	// linear impulse calculation easy imulse times nverse mass times contact normal direction
	//velocityChange[0] = impulse* m_vContactNormal * m_aEntitiesContacted[0]->GetPhysicsPointer()->getInverseMass();
	velocityChange[0] = impulse * m_aEntitiesContacted[0]->GetPhysicsPointer()->getInverseMass();

	// Apply the changes
	// gotta apply em or they aren't really very useful
	// but anyway easy update for the linear velocity
	m_aEntitiesContacted[0]->GetPhysicsPointer()->m_LinearVelocity+=velocityChange[0];
	// now the rotational velocity is a bit tricky as it ends up being applied to the orientation in the for of a quaternion
	// in local space which means we need to convert the impulse from world space to local so that when the quaternion
	// updates it will be updating correctly
	m_aEntitiesContacted[0]->GetPhysicsPointer()->m_RotationalVelocity+= m_aEntitiesContacted[0]->GetPhysicsPointer()->TransformMatrix.TransformInverseDirection(rotationChange[0]);
	//m_aEntitiesContacted[0]->GetPhysicsPointer()->m_RotationalVelocity+=rotationChange[0];
	// now we check if the other body is a world object if it is we skip it
	if (m_aEntitiesContacted[1]->GetPhysicsPointer()->mI_PhysicsType)
	{
		// now the impulse will be reversed for object two as its impulse will be aplied in the opposite direction gotta conserve those laws of physics
		impulse*=-1.0f;
		// we do the same things as above only this time to the other entity
		//DragonXVector3 impulsiveTorque = relitiveContactPosition[1]%(impulse * m_vContactNormal) ;
		DragonXVector3 impulsiveTorque = relitiveContactPosition[1]%impulse;
		rotationChange[1] = inverseInertiaTensor[1].TransformDirection(impulsiveTorque);
		velocityChange[1].clear();
		//velocityChange[1]= impulse * m_vContactNormal* m_aEntitiesContacted[1]->GetPhysicsPointer()->getInverseMass();
		velocityChange[1]= impulse * m_aEntitiesContacted[1]->GetPhysicsPointer()->getInverseMass();

		// And apply them.
		m_aEntitiesContacted[1]->GetPhysicsPointer()->m_LinearVelocity+=velocityChange[1];
		m_aEntitiesContacted[1]->GetPhysicsPointer()->addRotation(m_aEntitiesContacted[1]->GetPhysicsPointer()->TransformMatrix.TransformInverseDirection(rotationChange[1]));
	}
}
inline DragonXVector3 DragonContact::calculateFrictionlessImpulse(DragonXMatrix *inverseInertiaTensor)
{
	// here we calculate the impulse
	// we use a vector for the return type as a vector3 impulse is used in friction impulse calculations so we
	// leave it as a vector 3 for now but may change it to just scalar impulse if its decided friction is
	// useless, and this is just a local variable to help make our lives just a bit easier
	DragonXVector3 impulseDragonContact;
	// ok this is going to basically be an iterative inertia adding up thingy
	// we will start by calculating the inertia for body 1
	// first up relitive contact position cross contact normal
	DragonXVector3 deltaVelWorld = relitiveContactPosition[0] % m_vContactNormal;
	// transform by the inverse inertia tensor which is a mouthful
	// ill just call it tensor
	deltaVelWorld = inverseInertiaTensor[0].TransformDirection(deltaVelWorld);
	// cross with the relitive contact position again
	deltaVelWorld = deltaVelWorld % relitiveContactPosition[0];
	// then dot with contact normal
	real deltaVelocity = deltaVelWorld * m_vContactNormal;
	// lastly we add in the linear component of the inertia with the inverse mass
	deltaVelocity += m_aEntitiesContacted[0]->GetPhysicsPointer()->getInverseMass();
	// Check if body 2 is a world object
	if (m_aEntitiesContacted[1]->GetPhysicsPointer()->mI_PhysicsType)
	{
		// yup ok add in its stuff too
		// Go through the same transformation sequence again
		DragonXVector3 deltaVelWorld = relitiveContactPosition[1] % m_vContactNormal;
		deltaVelWorld = inverseInertiaTensor[1].TransformDirection(deltaVelWorld);
		deltaVelWorld = deltaVelWorld % relitiveContactPosition[1];
		deltaVelocity += deltaVelWorld * m_vContactNormal;
		deltaVelocity += m_aEntitiesContacted[1]->GetPhysicsPointer()->getInverseMass();
	}
	//now the finale divide expected change in velocity by the very long divisor of inertia
	// again we use a vector 3 in case we need to switch to friction based impulse
	// but 4 now ill just set x to impulse n use it
	impulseDragonContact.x=(desiredDeltaVelocity / deltaVelocity);
	impulseDragonContact.y=(0);
	impulseDragonContact.z=(0);
	return impulseDragonContact;
}
inline DragonXVector3 DragonContact::calculateFrictionImpulse(DragonXMatrix * inverseInertiaTensor)
{
	DragonXVector3 impulseDragonContact;
	real inverseMass = m_aEntitiesContacted[0]->GetPhysicsPointer()->getInverseMass();
	// The equivalent of a cross product in matrices is multiplication
	// by a skew symmetric matrix - we build the matrix for converting
	// between linear and angular quantities.
	DragonXMatrix impulseToTorque;
	impulseToTorque.setSkewSymmetric(relitiveContactPosition[0]);
	// Build the matrix to convert DragonContact impulse to change in velocity
	// in world coordinates.
	DragonXMatrix deltaVelWorld = impulseToTorque;
	deltaVelWorld *= inverseInertiaTensor[0];
	deltaVelWorld *= impulseToTorque;
	deltaVelWorld *= -1;
	// Check if we need to add m_aEntitiesContacted two's data
	if (m_aEntitiesContacted[1]->GetPhysicsPointer()->mI_PhysicsType)
	{
		// Set the cross product matrix
		impulseToTorque.setSkewSymmetric(relitiveContactPosition[1]);
		// Calculate the velocity change matrix
		DragonXMatrix deltaVelWorld2 = impulseToTorque;
		deltaVelWorld2 *= inverseInertiaTensor[1];
		deltaVelWorld2 *= impulseToTorque;
		deltaVelWorld2 *= -1;
		// Add to the total delta velocity.
		deltaVelWorld += deltaVelWorld2;
		// Add to the inverse mass
		inverseMass += m_aEntitiesContacted[1]->GetPhysicsPointer()->getInverseMass();
	}
	// Do a change of basis to convert into DragonContact coordinates.
	DragonXMatrix deltaVelocity = contactToWorld.transpose();
	deltaVelocity *= deltaVelWorld;
	deltaVelocity *= contactToWorld;
	// Add in the linear velocity change
	deltaVelocity._11 += inverseMass;
	deltaVelocity._22 += inverseMass;
	deltaVelocity._33 += inverseMass;
	// Invert to get the impulse needed per unit velocity
	DragonXMatrix impulseMatrix = deltaVelocity.getInverse();
	// Find the target velocities to kill
	DragonXVector3 velKill(desiredDeltaVelocity,
		-contactVelocity.y,
		-contactVelocity.z);
	// Find the impulse to kill target velocities
	impulseDragonContact = impulseMatrix.TransformDirection(velKill);
	// Check for exceeding friction
	real planarImpulse = (real)sqrt(
		impulseDragonContact.y*impulseDragonContact.y +
		impulseDragonContact.z*impulseDragonContact.z
		);
	if (planarImpulse > impulseDragonContact.x * m_rFriction)//this friction would be static friction below it would be dynamic friction
	{
		// We need to use dynamic friction
		impulseDragonContact.y=( impulseDragonContact.y/ planarImpulse);
		impulseDragonContact.z=( impulseDragonContact.z/ planarImpulse);
		impulseDragonContact.x=( deltaVelocity._11 +
			deltaVelocity._12*m_rFriction*impulseDragonContact.y +
			deltaVelocity._13*m_rFriction*impulseDragonContact.z);
		impulseDragonContact.x=( desiredDeltaVelocity / impulseDragonContact.x);
		impulseDragonContact.y=( impulseDragonContact.y * m_rFriction * impulseDragonContact.x);
		impulseDragonContact.z=( impulseDragonContact.z * m_rFriction * impulseDragonContact.x);
	}
	return impulseDragonContact;
}
void DragonContact::applyPositionChange(DragonXVector3 linearChange[2],DragonXVector3 angularChange[2],	real penetration)
{
	// ok here is the position or interpenitration fix method
	// first a few local variables to make our life easier
	const real angularLimit = (real)0.01f;
	real angularMove[2];
	real linearMove[2];
	real totalInertia = 0;
	real linearInertia[2];
	real angularInertia[2];
	// ok step one of interpenitration fixes with rotation is to ge the total inertia
	// and while we are at it we can obtain and hold on to various important parts of the inertia for use in a bit
	for (unsigned i = 0; i < 2; i++) if (m_aEntitiesContacted[i]->GetPhysicsPointer()->mI_PhysicsType)
	{
		// first up we are gunnaa cache the tensors
		DragonXMatrix inverseInertiaTensor;
		inverseInertiaTensor=m_aEntitiesContacted[i]->GetPhysicsPointer()->getInverseInertiaTensorWorld();
		// same as before we add up all the inertia
		DragonXVector3 angularInertiaWorld = relitiveContactPosition[i] % m_vContactNormal;
		angularInertiaWorld = inverseInertiaTensor.TransformDirection(angularInertiaWorld);
		angularInertiaWorld = angularInertiaWorld % relitiveContactPosition[i];
		angularInertia[i] = angularInertiaWorld * m_vContactNormal;
		linearInertia[i] = m_aEntitiesContacted[i]->GetPhysicsPointer()->getInverseMass();
		// Keep track of the total inertia from all components
		// we will need this in a moment
		totalInertia += linearInertia[i] + angularInertia[i];
		// We break the loop here so that the totalInertia value is
		// completely calculated (by both iterations) before
		// continuing.
	}
	// Loop through again calculating and applying the changes
	for (unsigned i = 0; i < 2; i++) if (m_aEntitiesContacted[i]->GetPhysicsPointer()->mI_PhysicsType)
	{
		// The linear and angular movements required are in proportion to
		// the two inverse inertias.
		// this lil bit of code sign basically says
		// if i ==0 sign = 1 if i!=0 sign = -1
		// what that means for us is
		//we dont have to worry about the multiplication of everything by -1 this will do it for us
		real sign =(i == 0)?(real)1:-1;
		angularMove[i] = sign * penetration * (angularInertia[i] / totalInertia);
		linearMove[i] = sign * penetration * (linearInertia[i] / totalInertia);

		// this lil bit of code
		DragonXVector3 proj = relitiveContactPosition[i];
		proj.operator+=(m_vContactNormal*(relitiveContactPosition[i]*-1.0f*m_vContactNormal));

		real maxMag= angularLimit * proj.GetMagnitude();

		if(angularMove[i] < -maxMag)
		{
			real totalMove = angularMove[i] + linearMove[i];
			angularMove[i] = -maxMag;
			linearMove[i]= totalMove-angularMove[i];
		}
		if(angularMove[i] > maxMag)
		{
			real totalMove = angularMove[i] + linearMove[i];
			angularMove[i] = maxMag;
			linearMove[i]= totalMove-angularMove[i];
		}

		if (angularMove[i] == 0)
		{
			// Easy case - no angular movement means no rotation.
			angularChange[i].clear();
		}
		else
		{
			// ok for rotation fixes first we determine what direction we want to rotate
			// we obtain this by crossing the contact point by the contact normal this way we rotate
			// in a direction perpendicular to the direction of the contact and the contact point itself relative
			// to the object we are rotating
			DragonXVector3 targetAngularDirection = relitiveContactPosition[i]%m_vContactNormal;
			// so that we want to ratate based on the tensor so that it rotates based on physics
			// so we need to transform the direction based on the tensor
			DragonXMatrix inverseInertiaTensor =  m_aEntitiesContacted[i]->GetPhysicsPointer()->getInverseInertiaTensorWorld();
			// ok now we need to rotate in this direction by the amount we determined above which is based
			// on the angular inertia and the total inertia and the penitration depth
			angularChange[i] = 	inverseInertiaTensor.TransformDirection(targetAngularDirection) * angularMove[i];
		}
		// Velocity change is easier - it is just the linear movement
		// along the DragonContact normal.
		linearChange[i] = m_vContactNormal * linearMove[i];
		// Now we can start to apply the values we've calculated.
		// Apply the linear movement
		m_aEntitiesContacted[i]->GetPhysicsPointer()->m_Position+=linearChange[i];
		// And the change in orientation
		// note this is done in world so we convert back using rotation part of the transform matrix
		m_aEntitiesContacted[i]->GetPhysicsPointer()->m_QuatRot.addScaledVectorDrX(m_aEntitiesContacted[i]->GetPhysicsPointer()->TransformMatrix.TransformInverseDirection(angularChange[i]),(real)1.0f);
		m_aEntitiesContacted[i]->GetPhysicsPointer()->m_QuatRot.normalize();
		// We need to calculate the derived data for any m_aEntitiesContacted that is
		// asleep, so that the changes are reflected in the object's
		// data. Otherwise the resolution will not change the position
		// of the object, and the next collision detection round will
		// have the same penetration.
		if (!m_aEntitiesContacted[i]->GetPhysicsPointer()->getAwake())
		{
			m_aEntitiesContacted[i]->GetPhysicsPointer()->UpdateMatricies();
		}
	}
}