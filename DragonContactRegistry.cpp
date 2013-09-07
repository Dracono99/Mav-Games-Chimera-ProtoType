#include "DragonContactRegistry.h"
#include "DragonEnums.h"
#include <cassert>

const static real REAL_MAX=9999999.99f;// this is a static const used locally though it is technically a global variable or rather a global const so yes its global scope but its unmodifyable

// helper functions
// this is a helper function that basically performs the dot product of the vector 3 axis passed in with the half sizes of the box passed in
// the dot product resultant being the projection of one vector 3 or in this case the axis of one box on to the axis of the box thats passed in
// the return value is the result of these dot products
static inline real transformToAxis( BaseGameEntity &box, const DragonXVector3 &axis)
{
	return box.GetPhysicsPointer()->GetHalfSizes().x * (real)abs(axis * box.GetPhysicsPointer()->getTransform().getAxisVector(0)) +
		box.GetPhysicsPointer()->GetHalfSizes().y * (real)abs(axis * box.GetPhysicsPointer()->getTransform().getAxisVector(1)) +
		box.GetPhysicsPointer()->GetHalfSizes().z * (real)abs(axis * box.GetPhysicsPointer()->getTransform().getAxisVector(2));
}

// checks if the 2 obbs overlap along the given axis toCentre is just passed to avoid recalculating it
static inline real penetrationOnAxis(
	BaseGameEntity &one,
	BaseGameEntity &two,
	const DragonXVector3 &axis,
	const DragonXVector3 &toCentre
	)
{
	// Project the half-size of one onto axis
	real oneProject = transformToAxis(one, axis);
	real twoProject = transformToAxis(two, axis);

	// Project this onto the axis
	real distance = (real)abs(toCentre * axis);

	// Return the overlap (i.e. positive indicates
	// overlap, negative indicates separation).
	return oneProject + twoProject - distance;
}

static inline bool tryAxis(
	BaseGameEntity &one,
	BaseGameEntity &two,
	DragonXVector3& axis,
	const DragonXVector3& toCentre,
	unsigned index,

	// These values may be updated
	real& smallestPenetration,
	unsigned &smallestCase
	)
{
	// Make sure we have a normalized axis, and don't check almost parallel axes
	if (axis.GetMagSquared() < 0.0001)
	{
		return true;
	}
	axis.Normalize();

	real penetration = penetrationOnAxis(one, two, axis, toCentre);

	if (penetration < 0) return false;
	if (penetration < smallestPenetration) {
		smallestPenetration = penetration;
		smallestCase = index;
	}
	return true;
}

void DragonContactRegistry::fillPointFaceBoxBox(BaseGameEntity &one,BaseGameEntity &two,const DragonXVector3 &toCentre,unsigned best,real pen)
{
	// This method is called when we know that a vertex from
	// box two is in contact with box one.
	DragonContact *DC = new DragonContact();
	// We know which axis the collision is on (i.e. best),
	// but we need to work out which of the two faces on
	// this axis.
	DragonXVector3 normal = one.GetPhysicsPointer()->getTransform().getAxisVector(best);
	if (one.GetPhysicsPointer()->getTransform().getAxisVector(best) * toCentre > 0)//changed>to<
	{
		normal.invert();
	}
	// Work out which vertex of box two we're colliding with.
	// Using toCentre doesn't work!
	DragonXVector3 vertex = two.GetPhysicsPointer()->GetHalfSizes();
	if (two.GetPhysicsPointer()->getTransform().getAxisVector(0) * normal < 0)//like wise changed
	{
		vertex.x = -vertex.x;
	}
	if (two.GetPhysicsPointer()->getTransform().getAxisVector(1) * normal < 0)//n again
	{
		vertex.y = -vertex.y;
	}
	if (two.GetPhysicsPointer()->getTransform().getAxisVector(2) * normal < 0)//n again
	{
		vertex.z = -vertex.z;
	}
	// Create the contact data
	DC->m_vContactNormal = normal;

	DC->m_rPenetration = pen;
	//DC->m_rPenetration = m_tolerence+pen;
	DC->m_vContactPoint = two.GetPhysicsPointer()->getTransform() * vertex;
	DC->SetContactData(&one, &two,m_Restitution);
	m_vContacts.push_back(DC);
}
// another helper function devoted to obb obb
static inline DragonXVector3 contactPoint(
	const DragonXVector3 &pOne,
	const DragonXVector3 &dOne,
	real oneSize,
	const DragonXVector3 &pTwo,
	const DragonXVector3 &dTwo,
	real twoSize,

	// If this is true, and the contact point is outside
	// the edge (in the case of an edge-face contact) then
	// we use one's midpoint, otherwise we use two's.
	bool useOne)
{
	DragonXVector3 toSt, cOne, cTwo;
	real dpStaOne, dpStaTwo, dpOneTwo, smOne, smTwo;
	real denom, mua, mub;

	smOne = dOne.GetMagSquared();
	smTwo = dTwo.GetMagSquared();
	dpOneTwo = dTwo * dOne;

	toSt = pOne - pTwo;
	dpStaOne = dOne * toSt;
	dpStaTwo = dTwo * toSt;

	denom = smOne * smTwo - dpOneTwo * dpOneTwo;

	// Zero denominator indicates parallel lines
	if ((real)abs(denom) < 0.0001f) {
		return useOne?pOne:pTwo;
	}

	mua = (dpOneTwo * dpStaTwo - smTwo * dpStaOne) / denom;
	mub = (smOne * dpStaTwo - dpOneTwo * dpStaOne) / denom;

	// If either of the edges has the nearest point out
	// of bounds, then the edges aren't crossed, we have
	// an edge-face contact. Our point is on the edge, which
	// we know from the useOne parameter.
	if (mua > oneSize ||
		mua < -oneSize ||
		mub > twoSize ||
		mub < -twoSize)
	{
		return useOne?pOne:pTwo;
	}
	else
	{
		cOne = pOne + dOne * mua;
		cTwo = pTwo + dTwo * mub;
		return cOne * 0.5 + cTwo * 0.5;
	}
}
//some more preprocessor stuff form optimizing obb obb contact generation
// This preprocessor definition is only used as a convenience
// in the boxAndBox contact generation method.
#define CHECK_OVERLAP(axis, index) \
	if (!tryAxis(one, two, (axis), toCentre, (index), pen, best)) return;

void DragonContactRegistry::ObbObbContactGen(BaseGameEntity &one,BaseGameEntity &two)
{
	//if (!IntersectionTests::boxAndBox(one, two)) return 0;

	// Find the vector between the two centers
	DragonXVector3 toCentre = two.GetPhysicsPointer()->getPosition() - one.GetPhysicsPointer()->getPosition();
	//DragonXVector3 toCentre = one.GetPhysicsPointer()->getPosition()-two.GetPhysicsPointer()->getPosition();

	// We start assuming there is no contact
	real pen = REAL_MAX;
	unsigned best = 0xffffff;

	// Now we check each axes, returning if it gives us
	// a separating axis, and keeping track of the axis with
	// the smallest penetration otherwise.
	CHECK_OVERLAP(one.GetPhysicsPointer()->getTransform().getAxisVector(0), 0);
	CHECK_OVERLAP(one.GetPhysicsPointer()->getTransform().getAxisVector(1), 1);
	CHECK_OVERLAP(one.GetPhysicsPointer()->getTransform().getAxisVector(2), 2);

	CHECK_OVERLAP(two.GetPhysicsPointer()->getTransform().getAxisVector(0), 3);
	CHECK_OVERLAP(two.GetPhysicsPointer()->getTransform().getAxisVector(1), 4);
	CHECK_OVERLAP(two.GetPhysicsPointer()->getTransform().getAxisVector(2), 5);

	// Store the best axis-major, in case we run into almost
	// parallel edge collisions later
	unsigned bestSingleAxis = best;

	CHECK_OVERLAP(one.GetPhysicsPointer()->getTransform().getAxisVector(0) % two.GetPhysicsPointer()->getTransform().getAxisVector(0), 6);
	CHECK_OVERLAP(one.GetPhysicsPointer()->getTransform().getAxisVector(0) % two.GetPhysicsPointer()->getTransform().getAxisVector(1), 7);
	CHECK_OVERLAP(one.GetPhysicsPointer()->getTransform().getAxisVector(0) % two.GetPhysicsPointer()->getTransform().getAxisVector(2), 8);
	CHECK_OVERLAP(one.GetPhysicsPointer()->getTransform().getAxisVector(1) % two.GetPhysicsPointer()->getTransform().getAxisVector(0), 9);
	CHECK_OVERLAP(one.GetPhysicsPointer()->getTransform().getAxisVector(1) % two.GetPhysicsPointer()->getTransform().getAxisVector(1), 10);
	CHECK_OVERLAP(one.GetPhysicsPointer()->getTransform().getAxisVector(1) % two.GetPhysicsPointer()->getTransform().getAxisVector(2), 11);
	CHECK_OVERLAP(one.GetPhysicsPointer()->getTransform().getAxisVector(2) % two.GetPhysicsPointer()->getTransform().getAxisVector(0), 12);
	CHECK_OVERLAP(one.GetPhysicsPointer()->getTransform().getAxisVector(2) % two.GetPhysicsPointer()->getTransform().getAxisVector(1), 13);
	CHECK_OVERLAP(one.GetPhysicsPointer()->getTransform().getAxisVector(2) % two.GetPhysicsPointer()->getTransform().getAxisVector(2), 14);

	// Make sure we have got a result.
	assert(best != 0xffffff);

	// We now know there's a collision, and we know which
	// of the axes gave the smallest penetration. We now
	// can deal with it in different ways depending on
	// the case.
	if (best < 3)
	{
		// We've got a vertex of box two on a face of box one.
		fillPointFaceBoxBox(one, two, toCentre,best, pen);
		//fillPointFaceBoxBox(two, one, toCentre,best, pen);
	}
	else if (best < 6)
	{
		// We've got a vertex of box one on a face of box two.
		// We use the same algorithm as above, but swap around
		// one and two (and therefore also the vector between their
		// centers).
		fillPointFaceBoxBox(two, one, toCentre*-1.0f, best-3, pen);
		//fillPointFaceBoxBox(one, two,toCentre*1.0f, best-3, pen)
	}
	else
	{
		// We've got an edge-edge contact. Find out which axes
		best -= 6;
		unsigned oneAxisIndex = best / 3;
		unsigned twoAxisIndex = best % 3;
		DragonXVector3 oneAxis = one.GetPhysicsPointer()->getTransform().getAxisVector(oneAxisIndex);
		DragonXVector3 twoAxis = two.GetPhysicsPointer()->getTransform().getAxisVector(twoAxisIndex);
		DragonXVector3 axis = oneAxis % twoAxis;
		axis.Normalize();

		// The axis should point from box one to box two.
		if (axis * toCentre > 0)
		{
			axis = axis * -1.0f;
		}

		// We have the axes, but not the edges: each axis has 4 edges parallel
		// to it, we need to find which of the 4 for each object. We do
		// that by finding the point in the center of the edge. We know
		// its component in the direction of the box's collision axis is zero
		// (its a mid-point) and we determine which of the extremes in each
		// of the other axes is closest.
		DragonXVector3 ptOnOneEdge = one.GetPhysicsPointer()->GetHalfSizes();
		DragonXVector3 ptOnTwoEdge = two.GetPhysicsPointer()->GetHalfSizes();
		for (unsigned i = 0; i < 3; i++)
		{
			if (i == oneAxisIndex)
			{
				ptOnOneEdge[i] = 0;
			}
			else if (one.GetPhysicsPointer()->getTransform().getAxisVector(i) * axis > 0)
			{
				ptOnOneEdge[i] = -ptOnOneEdge[i];
			}

			if (i == twoAxisIndex)
			{
				ptOnTwoEdge[i] = 0;
			}
			else if (two.GetPhysicsPointer()->getTransform().getAxisVector(i) * axis < 0)
			{
				ptOnTwoEdge[i] = -ptOnTwoEdge[i];
			}
		}

		// Move them into world coordinates (they are already oriented
		// correctly, since they have been derived from the axes).
		ptOnOneEdge = one.GetPhysicsPointer()->getTransform() * ptOnOneEdge;
		ptOnTwoEdge = two.GetPhysicsPointer()->getTransform() * ptOnTwoEdge;

		// So we have a point and a direction for the colliding edges.
		// We need to find out point of closest approach of the two
		// line-segments.
		DragonXVector3 vertex = contactPoint(
			ptOnOneEdge, oneAxis, one.GetPhysicsPointer()->GetHalfSizes()[oneAxisIndex],
			ptOnTwoEdge, twoAxis, two.GetPhysicsPointer()->GetHalfSizes()[twoAxisIndex],
			bestSingleAxis > 2
			);

		// We can fill the contact.
		DragonContact *DC = new DragonContact();

		DC->m_rPenetration = pen;
		//DC->m_rPenetration = m_tolerence+pen;
		DC->m_vContactNormal = axis;
		DC->m_vContactPoint = vertex;
		DC->SetContactData(&one, &two, m_Restitution);
		m_vContacts.push_back(DC);
	}
}
#undef CHECK_OVERLAP

static inline real DistPointPlane(DragonXVector3& p, DragonXVector3& planeNormal,real planeDist)
{
	return p*planeNormal-planeDist;
}

static inline bool EdgeEdgeTest(int i0,int i1,real ax,real ay,DragonXVector3& v0, DragonXVector3& u0, DragonXVector3& u1)
{
	real bx = u0[i0]-u1[i0];
	real by = u0[i1]-u1[i1];
	real cx = v0[i0]-u0[i0];
	real cy = v0[i1]-u0[i1];
	real f = ay*bx-ax*by;
	real d = by*cx-bx*cy;
	if((f>0.0f&&d<=f)||(f<0&&d<=0&&d>=f))
	{
		real e = ax*cy-ay*cx;
		if(f>0)
		{
			if(e>=0&&e<=f)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			if(e<=0&&e>=f)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	}
	else
	{
		return false;
	}
}

static inline bool EdgeAgainstTriEdges(int i0,int i1,DragonXVector3& v0,DragonXVector3& v1, DragonXVector3& u0,DragonXVector3& u1, DragonXVector3& u2)
{
	real ax = v1[i0]-v0[i0];
	real ay = v1[i0] - v0[i1];

	if(EdgeEdgeTest(i0,i1,ax,ay,v0,u0,u1))
	{
		return true;
	}
	else if(EdgeEdgeTest(i0,i1,ax,ay,v0,u1,u2))
	{
		return true;
	}
	else if(EdgeEdgeTest(i0,i1,ax,ay,v0,u2,u0))
	{
		return true;
	}
	else
	{
		return false;
	}
}

static inline bool PointInTri(int i0, int i1, DragonXVector3& v0,DragonXVector3& u0,DragonXVector3& u1, DragonXVector3& u2)
{
	real a = u1[i1]-u0[i1];
	real b=-(u1[i0]-u0[i0]);
	real c=-a*u0[i0]-b*u0[i1];
	real d0=a*v0[i0]+b*v0[i1]+c;

	a = u2[i1]-u1[i1];
	b=-(u2[i0]-u1[i0]);
	c=-a*u1[i0]-b*u1[i1];
	real d1=a*v0[i0]+b*v0[i1]+c;

	a = u0[i1]-u2[i1];
	b=-(u0[i0]-u2[i0]);
	c=-a*u2[i0]-b*u2[i1];
	real d2=a*v0[i0]+b*v0[i1]+c;

	if(d0*d1>0.0f)
	{
		if(d0*d2>0.0f)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

static inline bool CoplanarTriTest(DragonXVector3& norm,DragonXVector3& tri1a,DragonXVector3& tri1b,DragonXVector3& tri1c,DragonXVector3& tri2a,DragonXVector3& tri2b,DragonXVector3& tri2c)
{
	real a0 = (real)abs(norm.x);
	real a1 = (real)abs(norm.y);
	real a2 = (real)abs(norm.z);
	int i0 = 0;
	int i1 = 0;

	if(a0>a1)
	{
		if(a0>a2)
		{
			i0=1;
			i1=2;
		}
		else
		{
			i0=0;
			i1=1;
		}
	}
	else
	{
		if(a2>a1)
		{
			i0=0;
			i1=1;
		}
		else
		{
			i0=0;
			i1=2;
		}
	}

	if(EdgeAgainstTriEdges(i0,i1,tri1a,tri1b,tri2a,tri2b,tri2c))
	{
		return true;
	}
	else if(EdgeAgainstTriEdges(i0,i1,tri1b,tri1c,tri2a,tri2b,tri2c))
	{
		return true;
	}
	else if(EdgeAgainstTriEdges(i0,i1,tri1c,tri1a,tri2a,tri2b,tri2c))
	{
		return true;
	}
	else if(PointInTri(i0,i1,tri1a,tri2a,tri2b,tri2c))
	{
		return true;
	}
	else if(PointInTri(i0,i1,tri2a,tri1a,tri1b,tri1c))
	{
		return true;
	}
	else
	{
		return false;
	}
}
inline bool TestTriVertsAgainstPlane(DragonXVector3& vertA,DragonXVector3& vertB,DragonXVector3& vertC,DragonXVector3& planeNormal,real planeDist,real& d0,real& d1,real& d2)
{
	d0 = DistPointPlane(vertA,planeNormal,planeDist);
	d1 = DistPointPlane(vertB,planeNormal,planeDist);
	d2 = DistPointPlane(vertC,planeNormal,planeDist);

	if(d0>0&&d1>0&&d2>0)
	{
		return false;
	}
	else if(d0<0&&d1<0&&d2<0)
	{
		return false;
	}
	else
	{
		return true;
	}
}
inline real ScalarTriple(DragonXVector3& a, DragonXVector3& b, DragonXVector3& c)
{
	return c*(a%b);
}

static inline DragonXVector3 ClosestPtPointTriangle(DragonXVector3& p,DragonXVector3& a,DragonXVector3& b,DragonXVector3& c)
{
	// check if p is in vertex region outside a
	DragonXVector3 ab = b-a;
	DragonXVector3 ac = c-a;
	DragonXVector3 ap = p-a;
	real d1 = ab*ap;
	real d2 = ac*ap;
	if(d1<=0.0f&&d2<=0.0f)
	{
		return a;
	}

	// check b
	DragonXVector3 bp = p-b;
	real d3 = ab*bp;
	real d4=ac*bp;
	if(d3>=0.0f&&d4<=d3)
	{
		return b;
	}

	// check edge region ab
	real vc = (d1*d4)-(d3*d2);
	if(vc<=0.0f&&d1>=0.0f&&d3<=0.0f)
	{
		real v = d1/(d1-d3);
		return a+v*ab;
	}

	// check c
	DragonXVector3 cp = p-c;
	real d5=ab*cp;
	real d6=ac*cp;
	if(d6>=0.0f&&d5<=d6)
	{
		return c;
	}

	// check p on ac
	real vb = (d5*d2)-(d1*d6);
	if(vb<=0.0f&&d2>=0.0f&&d6<=0.0f)
	{
		real w = d2/(d2-d6);
		return a+w*ac;
	}

	// check p on bc
	real va = (d3*d6)-(d5*d4);
	if(va<=0.0f&&(d4-d3)>=0.0f&&(d5-d6)>=0.0f)
	{
		real w = (d4-d3)/((d4-d3)+(d5-d6));
		return b + w*(c-b);
	}

	// p is in the face region
	real denom = 1.0f/(va+vb+vc);
	real v = vb * denom;
	real w = vc * denom;
	return a+(ab*v)+(ac*w);
}

// this is a helper function i wrote for use with kdops but as they are still in developement its currently used
static inline void clostestPtPointOBB(DragonXVector3& p,BaseGameEntity* obb, DragonXVector3& q)
{
	DragonXVector3 d = p - obb->GetPhysicsPointer()->getPosition();
	q = obb->GetPhysicsPointer()->getPosition();
	DragonXVector3 he = obb->GetPhysicsPointer()->GetHalfSizes();
	//for each obb axis

	real dist = d * obb->GetPhysicsPointer()->getTransform().getAxisVector(0);
	// if the dist is further than the box extents clamp it
	if(dist> he.x)
	{
		dist = he.x;
	}
	if(dist < -he.x)
	{
		dist = -he.x;
	}
	q+=dist * obb->GetPhysicsPointer()->getTransform().getAxisVector(0);

	dist = d * obb->GetPhysicsPointer()->getTransform().getAxisVector(1);
	// if the dist is further than the box extents clamp it
	if(dist> he.y)
	{
		dist = he.y;
	}
	if(dist < -he.y)
	{
		dist = -he.y;
	}
	q+=dist * obb->GetPhysicsPointer()->getTransform().getAxisVector(1);

	dist = d * obb->GetPhysicsPointer()->getTransform().getAxisVector(2);
	// if the dist is further than the box extents clamp it
	if(dist> he.z)
	{
		dist = he.z;
	}
	if(dist < -he.z)
	{
		dist = -he.z;
	}
	q+=dist * obb->GetPhysicsPointer()->getTransform().getAxisVector(2);
}

// this is a helper function i wrote for use with kdops but as they are still in developement its currently used
static inline void clostestPtPointOBBOnObbNotIn(DragonXVector3& p,BaseGameEntity* obb, DragonXVector3& q)
{
	DragonXVector3 d = p - obb->GetPhysicsPointer()->getPosition();
	q = obb->GetPhysicsPointer()->getPosition();
	DragonXVector3 he = obb->GetPhysicsPointer()->GetHalfSizes();
	//for each obb axis

	real dist = d * obb->GetPhysicsPointer()->getTransform().getAxisVector(0);
	// if the dist is further than the box extents clamp it
	if(dist> he.x)
	{
		dist = he.x;
	}
	if(dist < -he.x)
	{
		dist = -he.x;
	}
	q+=dist * obb->GetPhysicsPointer()->getTransform().getAxisVector(0);

	dist = d * obb->GetPhysicsPointer()->getTransform().getAxisVector(1);
	// if the dist is further than the box extents clamp it
	if(dist> he.y)
	{
		dist = he.y;
	}
	if(dist < -he.y)
	{
		dist = -he.y;
	}
	q+=dist * obb->GetPhysicsPointer()->getTransform().getAxisVector(1);

	dist = d * obb->GetPhysicsPointer()->getTransform().getAxisVector(2);
	// if the dist is further than the box extents clamp it
	if(dist> he.z)
	{
		dist = he.z;
	}
	if(dist < -he.z)
	{
		dist = -he.z;
	}
	q+=dist * obb->GetPhysicsPointer()->getTransform().getAxisVector(2);
	bool on=false;
	if(q.x>=he.x||q.x<=-he.x)
	{
		on=true;
	}
	if(q.y>=he.y||q.y<=-he.y)
	{
		on=true;
	}
	if(q.z>=he.z||q.z<=-he.z)
	{
		on=true;
	}
	if(!on)
	{
		DragonXVector3 dif = obb->GetPhysicsPointer()->getPointInLocalSpace(q)-he;
		if((real)abs(dif.x)<(real)abs(dif.y)&&(real)abs(dif.x)<(real)abs(dif.z))
		{
			if(q.x<0)
			{
				q.x=obb->GetPhysicsPointer()->getPosition().x-he.x;
			}
			else
			{
				q.x=obb->GetPhysicsPointer()->getPosition().x+he.x;
			}
			return;
		}

		if((real)abs(dif.y)<(real)abs(dif.x)&&(real)abs(dif.y)<(real)abs(dif.z))
		{
			if(q.y<0)
			{
				q.y=obb->GetPhysicsPointer()->getPosition().y-he.y;
			}
			else
			{
				q.y=obb->GetPhysicsPointer()->getPosition().y+he.y;
			}
			return;
		}

		if((real)abs(dif.z)<(real)abs(dif.y)&&(real)abs(dif.z)<(real)abs(dif.x))
		{
			if(q.z<0)
			{
				q.z=obb->GetPhysicsPointer()->getPosition().z-he.z;
			}
			else
			{
				q.z=obb->GetPhysicsPointer()->getPosition().z+he.z;
			}
			return;
		}
	}
}

static inline bool SphereSphereIntersectTest(DragonXVector3& p1,real r1,DragonXVector3& p2,real r2)
{
	return (p1-p2).GetMagSquared()<(r1+r2)*(r1+r2);
}

// note verts should be in ccw order dragon tri verts are clock wise so when passing in verts reverse the order so c b a instead of abc
static inline DragonXVector3 TriEdgePointOfContact(DragonXVector3& p,DragonXVector3&q,DragonXVector3& a,DragonXVector3& b,DragonXVector3& c)
{
	DragonXVector3 pq = q-p;
	DragonXVector3 m = pq%p;
	real u = (pq*(c%b))+(m*(c-b));
	real v = (pq*(a%c))+(m*(a-c));
	real w = (pq*(b%a))+(m*(b-a));
	real denom=1.0f/(u+v+w);
	u*=denom;
	v*=denom;
	w*=denom;
	DragonXVector3 ret = u*a+v*b+w*c;
	return ret;
}

DragonContactRegistry::DragonContactRegistry()
{
	m_Restitution=0.50f;
	//m_tolerence=0.1;
	m_iLimit=2500;
}

DragonContactRegistry::~DragonContactRegistry()
{
	ClearPairs();
	ClearContacts();
	ClearRayContacts();
}

int DragonContactRegistry::AddNaturalContactPair(int collisionType, BaseGameEntity* obj1, BaseGameEntity* obj2)
{
	if(m_vContactPairs.size()>0)
	{
		bool add = true;
		for(auto pair : m_vContactPairs)
		{
			if(pair->obj1==obj1&&pair->obj2==obj2)
			{
				add=false;
			}
			else if(pair->obj1==obj2&&pair->obj2==obj1)
			{
				add=false;
			}
		}
		if(add)
		{
			DragonicPair* p = new DragonicPair;
			p->mCollisionType=collisionType;
			p->mContactregistryType=CRT_Natural;
			p->obj1=obj1;
			p->obj2=obj2;
			GenerateContact(p->mCollisionType,p->obj1,p->obj2);
			m_vContactPairs.push_back(p);
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		DragonicPair* p = new DragonicPair;
		p->mCollisionType=collisionType;
		p->mContactregistryType=CRT_Natural;
		p->obj1=obj1;
		p->obj2=obj2;
		GenerateContact(p->mCollisionType,p->obj1,p->obj2);
		m_vContactPairs.push_back(p);
		return 1;
	}
}
int DragonContactRegistry::AddRayContactPair(int collisionType, BaseGameEntity* obj1, BaseGameEntity* obj2)
{
	if(m_vContactPairs.size()>0)
	{
		bool add = true;
		for(auto pair : m_vContactPairs)
		{
			if(pair->obj1==obj1&&pair->obj2==obj2)
			{
				add=false;
			}
			else if(pair->obj1==obj2&&pair->obj2==obj1)
			{
				add=false;
			}
		}
		if(add)
		{
			DragonicPair* p = new DragonicPair;
			p->mCollisionType=collisionType;
			p->mContactregistryType=CRT_Ray;
			p->obj1=obj1;
			p->obj2=obj2;
			GenerateContact(p->mCollisionType,p->obj1,p->obj2);
			m_vContactPairs.push_back(p);
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		DragonicPair* p = new DragonicPair;
		p->mCollisionType=collisionType;
		p->mContactregistryType=CRT_Ray;
		p->obj1=obj1;
		p->obj2=obj2;
		GenerateContact(p->mCollisionType,p->obj1,p->obj2);
		m_vContactPairs.push_back(p);
		return 1;
	}
}

void DragonContactRegistry::ResolveContacts(DragonContactResolver* resolver,real dt)
{
	resolver->setIterations(m_vContacts.size()*4);
	resolver->resolveContacts(m_vContacts,m_vContacts.size(),dt);
	UpdateContactList(resolver->GetEpsilon());
	//ClearContacts();
}

void DragonContactRegistry::UpdateContactList(real threshold)
{
	// this makes a temp copy of all the contact pairs
	std::vector<DragonicPair*> temp=m_vContactPairs;
	// this is a copy of the individual contacts
	std::vector<DragonContact*> tempC=m_vContacts;
	// this is a copy of ray contacts
	std::vector<DragonContact*> tempR=m_RayContacts;
	// clear ray contacts
	ClearRayContacts();
	// this clears the source vector of contacts
	ClearContacts();
	// this simply clears the vector of contact pairs
	m_vContactPairs.clear();
	// this checks if the contact pair has had its contact(s) resolved if it has it will be removed if not it will have its contacts recreated
	for(const auto DragonicPair : temp)
	{
		if(DragonicPair->mContactregistryType==CRT_Natural)
		{
			for(const auto DragonContact : tempC)
			{
				if(DragonContact->m_aEntitiesContacted[0]==DragonicPair->obj1)
				{
					if(DragonContact->m_aEntitiesContacted[1]==DragonicPair->obj2)
					{
						if(DragonContact->m_rPenetration>=threshold)
						{
							AddNaturalContactPair(DragonicPair->mCollisionType,DragonicPair->obj1,DragonicPair->obj2);
						}
					}
				}
				else if(DragonContact->m_aEntitiesContacted[1]==DragonicPair->obj1)
				{
					if(DragonContact->m_aEntitiesContacted[0]==DragonicPair->obj2)
					{
						if(DragonContact->m_rPenetration>=threshold)
						{
							AddNaturalContactPair(DragonicPair->mCollisionType,DragonicPair->obj2,DragonicPair->obj1);
						}
					}
				}
			}
		}
		else
		{
			for(const auto DragonContact : tempR)
			{
				if(DragonContact->m_aEntitiesContacted[0]==DragonicPair->obj1)
				{
					if(DragonContact->m_aEntitiesContacted[1]==DragonicPair->obj2)
					{
						if(DragonContact->m_rPenetration>=0.0f)
						{
							AddRayContactPair(DragonicPair->mCollisionType,DragonicPair->obj1,DragonicPair->obj2);
						}
					}
				}
				else if(DragonContact->m_aEntitiesContacted[1]==DragonicPair->obj1)
				{
					if(DragonContact->m_aEntitiesContacted[0]==DragonicPair->obj2)
					{
						if(DragonContact->m_rPenetration>=0.0f)
						{
							AddRayContactPair(DragonicPair->mCollisionType,DragonicPair->obj2,DragonicPair->obj1);
						}
					}
				}
			}
		}
	}
	// this is no longer needed so its destroyed
	for(const auto DragonicPair : temp)
	{
		delete DragonicPair;
	}
}
void DragonContactRegistry::GenerateContactRaySphere(BaseGameEntity* obj1,BaseGameEntity* obj2)
{
	if (obj1->GetPhysicsPointer()->mI_PrimitiveType==PT_Ray)//if obj1 is the ray
	{
		DragonXVector3 m=obj1->GetPhysicsPointer()->getPosition()-obj2->GetPhysicsPointer()->getPosition();
		real b = m*obj1->GetPhysicsPointer()->GetNormal();
		real c = m*m-obj2->GetPhysicsPointer()->GetRadius()*obj2->GetPhysicsPointer()->GetRadius();
		DragonContact* DC = new DragonContact();
		DC->SetContactData(obj1,obj2,m_Restitution);
		if (c>0.0f&&b>0.0f)
		{
			DC->m_rPenetration=-1;
		}

		real discr = b*b-c;
		if(discr<0.0f)
		{
			DC->m_rPenetration=-1;
		}
		else
		{
			real t = -b-(real)sqrt(discr);
			if(t<0.0f)
			{
				t=0.0f;
			}
			DC->m_rPenetration=t;
		}
		DC->m_vContactPoint=obj1->GetPhysicsPointer()->getPosition()+DC->m_rPenetration*obj1->GetPhysicsPointer()->GetNormal();
		DC->m_vContactNormal=obj1->GetPhysicsPointer()->GetNormal();
		m_RayContacts.push_back(DC);
	}
	else// obj2 is the ray
	{
		DragonXVector3 m=obj2->GetPhysicsPointer()->getPosition()-obj1->GetPhysicsPointer()->getPosition();
		real b = m*obj2->GetPhysicsPointer()->GetNormal();
		real c = m*m-obj1->GetPhysicsPointer()->GetRadius()*obj1->GetPhysicsPointer()->GetRadius();
		DragonContact* DC = new DragonContact();
		DC->SetContactData(obj2,obj1,m_Restitution);
		if (c>0.0f&&b>0.0f)
		{
			DC->m_rPenetration=-1;
		}

		real discr = b*b-c;
		if(discr<0.0f)
		{
			DC->m_rPenetration=-1;
		}
		else
		{
			real t = -b-(real)sqrt(discr);
			if(t<0.0f)
			{
				t=0.0f;
			}
			DC->m_rPenetration=t;
		}
		DC->m_vContactPoint=obj2->GetPhysicsPointer()->getPosition()+DC->m_rPenetration*obj2->GetPhysicsPointer()->GetNormal();
		DC->m_vContactNormal=obj2->GetPhysicsPointer()->GetNormal();
		m_RayContacts.push_back(DC);
	}
}
void DragonContactRegistry::GenerateContactRayObb(BaseGameEntity* obj1,BaseGameEntity* obj2)
{
	real EPSILON = 0.001f;
	if (obj1->GetPhysicsPointer()->mI_PrimitiveType==PT_Ray)//if obj1 is ray
	{
		real tmin=-999999999.9f;
		real tmax=9999999999.9f;
		DragonXVector3 p = obj2->GetPhysicsPointer()->TransformMatrix.TransformInverse(obj1->GetPhysicsPointer()->getPosition());
		//DragonXVector3 d = obj1->GetPhysicsPointer()->GetNormal();
		DragonXVector3 d = obj2->GetPhysicsPointer()->TransformMatrix.TransformInverseDirection(obj1->GetPhysicsPointer()->GetNormal());
		if(abs(d.x)<EPSILON)// for x
		{
			if(p.x<-obj2->GetPhysicsPointer()->GetHalfSizes().x||p.x>obj2->GetPhysicsPointer()->GetHalfSizes().x)
			{
				return;
			}
		}
		else
		{
			real ood = 1.0f / d.x;
			real t1 = (-obj2->GetPhysicsPointer()->GetHalfSizes().x - p.x)*ood;
			real t2 = (obj2->GetPhysicsPointer()->GetHalfSizes().x - p.x)*ood;
			if(t1>t2)
			{
				real temp = t1;
				t1=t2;
				t2=temp;
			}
			if(t1>tmin)
			{
				tmin=t1;
			}
			if (t2>tmax)
			{
				tmax=t2;
			}
			if (tmin>tmax)
			{
				return;
			}
		}
		if(abs(d.y)<EPSILON)// for y
		{
			if(p.y<-obj2->GetPhysicsPointer()->GetHalfSizes().y||p.y>obj2->GetPhysicsPointer()->GetHalfSizes().y)
			{
				return;
			}
		}
		else
		{
			real ood = 1.0f / d.y;
			real t1 = (-obj2->GetPhysicsPointer()->GetHalfSizes().y - p.y)*ood;
			real t2 = (obj2->GetPhysicsPointer()->GetHalfSizes().y - p.y)*ood;
			if(t1>t2)
			{
				real temp = t1;
				t1=t2;
				t2=temp;
			}
			if(t1>tmin)
			{
				tmin=t1;
			}
			if (t2>tmax)
			{
				tmax=t2;
			}
			if (tmin>tmax)
			{
				return;
			}
		}
		if(abs(d.z)<EPSILON)// for z
		{
			if(p.z<-obj2->GetPhysicsPointer()->GetHalfSizes().z||p.z>obj2->GetPhysicsPointer()->GetHalfSizes().z)
			{
				return;
			}
		}
		else
		{
			real ood = 1.0f / d.z;
			real t1 = (-obj2->GetPhysicsPointer()->GetHalfSizes().z - p.z)*ood;
			real t2 = (obj2->GetPhysicsPointer()->GetHalfSizes().z - p.z)*ood;
			if(t1>t2)
			{
				real temp = t1;
				t1=t2;
				t2=temp;
			}
			if(t1>tmin)
			{
				tmin=t1;
			}
			if (t2>tmax)
			{
				tmax=t2;
			}
			if (tmin>tmax)
			{
				return;
			}
		}
		DragonContact* DC = new DragonContact();
		//DC->m_vContactPoint=obj1->GetPhysicsPointer()->getPosition()+d*tmin;
		DC->m_vContactPoint=obj2->GetPhysicsPointer()->getPointInWorldSpace(p+d*tmin);
		DC->m_vContactNormal=obj2->GetPhysicsPointer()->getDirectionInWorldSpace(d);
		DC->m_rPenetration=tmin;
		DC->SetContactData(obj1,obj2,m_Restitution);
		m_RayContacts.push_back(DC);
	}
	else// obj2 is the ray
	{
		real tmin=-999999999.9f;
		real tmax=9999999999.9f;
		DragonXVector3 p = obj1->GetPhysicsPointer()->TransformMatrix.TransformInverse(obj2->GetPhysicsPointer()->getPosition());
		DragonXVector3 d = obj1->GetPhysicsPointer()->TransformMatrix.TransformInverseDirection(obj2->GetPhysicsPointer()->GetNormal());
		if(abs(d.x)<EPSILON)// for x
		{
			if(p.x<-obj1->GetPhysicsPointer()->GetHalfSizes().x||p.x>obj1->GetPhysicsPointer()->GetHalfSizes().x)
			{
				return;
			}
		}
		else
		{
			real ood = 1.0f / d.x;
			real t1 = (-obj1->GetPhysicsPointer()->GetHalfSizes().x - p.x)*ood;
			real t2 = (obj1->GetPhysicsPointer()->GetHalfSizes().x - p.x)*ood;
			if(t1>t2)
			{
				real temp = t1;
				t1=t2;
				t2=temp;
			}
			if(t1>tmin)
			{
				tmin=t1;
			}
			if (t2>tmax)
			{
				tmax=t2;
			}
			if (tmin>tmax)
			{
				return;
			}
		}
		if(abs(d.y)<EPSILON)// for y
		{
			if(p.y<-obj1->GetPhysicsPointer()->GetHalfSizes().y||p.y>obj1->GetPhysicsPointer()->GetHalfSizes().y)
			{
				return;
			}
		}
		else
		{
			real ood = 1.0f / d.y;
			real t1 = (-obj1->GetPhysicsPointer()->GetHalfSizes().y - p.y)*ood;
			real t2 = (obj1->GetPhysicsPointer()->GetHalfSizes().y - p.y)*ood;
			if(t1>t2)
			{
				real temp = t1;
				t1=t2;
				t2=temp;
			}
			if(t1>tmin)
			{
				tmin=t1;
			}
			if (t2>tmax)
			{
				tmax=t2;
			}
			if (tmin>tmax)
			{
				return;
			}
		}
		if(abs(d.z)<EPSILON)// for z
		{
			if(p.z<-obj1->GetPhysicsPointer()->GetHalfSizes().z||p.z>obj1->GetPhysicsPointer()->GetHalfSizes().z)
			{
				return;
			}
		}
		else
		{
			real ood = 1.0f / d.z;
			real t1 = (-obj1->GetPhysicsPointer()->GetHalfSizes().z - p.z)*ood;
			real t2 = (obj1->GetPhysicsPointer()->GetHalfSizes().z - p.z)*ood;
			if(t1>t2)
			{
				real temp = t1;
				t1=t2;
				t2=temp;
			}
			if(t1>tmin)
			{
				tmin=t1;
			}
			if (t2>tmax)
			{
				tmax=t2;
			}
			if (tmin>tmax)
			{
				return;
			}
		}
		DragonContact* DC = new DragonContact();
		//DC->m_vContactPoint=obj2->GetPhysicsPointer()->getPosition()+d*tmin;
		DC->m_vContactPoint=obj1->GetPhysicsPointer()->getPointInWorldSpace(p+d*tmin);
		DC->m_vContactNormal=obj1->GetPhysicsPointer()->getDirectionInWorldSpace(d);
		DC->m_rPenetration=tmin;
		DC->SetContactData(obj2,obj1,m_Restitution);
		m_RayContacts.push_back(DC);
	}
}
void DragonContactRegistry::GenerateContact(int collisionType,BaseGameEntity*obj1,BaseGameEntity*obj2)
{
	// this generates contacts based on the entities
	switch(collisionType)
	{
	case CT_SphereSphere:
		{
			GenerateContactSphereSphere(obj1,obj2);
			break;
		}
	case CT_SpherePlane:
		{
			GenerateContactSpherePlane(obj1,obj2);
			break;
		}
	case CT_SphereOBB:
		{
			GenerateContactSphereOBB(obj1,obj2);
			break;
		}
	case CT_ObbPlane:
		{
			GenerateContactOBBPlane(obj1,obj2);
			break;
		}
	case CT_ObbObb:
		{
			GenerateContactOBBOBB(obj1,obj2);
			break;
		}
	case CT_RaySphere:
		{
			GenerateContactRaySphere(obj1,obj2);
			break;
		}
	case CT_RayOBB:
		{
			GenerateContactRayObb(obj1,obj2);
			break;
		}
	case CT_MeshSphere:
		{
			GenerateContactMeshSphere(obj1,obj2);
			break;
		}
	case CT_MeshOBB:
		{
			GenerateContactMeshObb(obj1,obj2);
			break;
		}
	case CT_MeshMesh:
		{
			GenerateContactsMeshMesh(obj1,obj2);
			break;
		}
	case CT_MeshRay:
		{
			GenerateContactsMeshRay(obj1,obj2);
			break;
		}
	default:
		{
			break;
		}
	}
}

void DragonContactRegistry::ClearContacts()
{
	for(const auto DragonContact : m_vContacts)
	{
		delete DragonContact;
	}
	m_vContacts.clear();
}

void DragonContactRegistry::GenerateContactSphereSphere(BaseGameEntity* obj1,BaseGameEntity* obj2)
{
	DragonContact *DC = new DragonContact();
	DragonXVector3 midLine(obj1->GetPhysicsPointer()->m_Position - obj2->GetPhysicsPointer()->m_Position);
	real size = midLine.GetMagnitude();
	DC->m_vContactNormal= midLine*((1.0f)/size);
	DC->m_vContactPoint=obj1->GetPhysicsPointer()->getPosition() +midLine * 0.5f;
	DC->m_rPenetration=obj1->GetPhysicsPointer()->GetRadius()+obj2->GetPhysicsPointer()->GetRadius() - size;
	DC->SetContactData(obj1,obj2,(real)m_Restitution);
	m_vContacts.push_back(DC);
}
// plane sphere
void DragonContactRegistry::GenerateContactSpherePlane(BaseGameEntity* obj1,BaseGameEntity* obj2)
{
	if(obj1->GetPhysicsPointer()->mI_PrimitiveType==PT_Sphere)
	{
		DragonContact *DC = new DragonContact();
		real centerDist = obj2->GetPhysicsPointer()->GetNormal() * obj1->GetPhysicsPointer()->getPosition() - obj2->GetPhysicsPointer()->GetDistFromOrigin();
		DragonXVector3 normal = obj2->GetPhysicsPointer()->GetNormal();
		real penetration = -centerDist;
		if(centerDist < 0)
		{
			normal.invert();
			penetration = -penetration;
		}
		penetration+=obj1->GetPhysicsPointer()->GetRadius();
		// fill out contact data
		DC->m_vContactNormal=normal;
		DC->m_rPenetration=penetration;
		DC->m_vContactPoint = obj1->GetPhysicsPointer()->getPosition() - obj2->GetPhysicsPointer()->GetNormal() * centerDist;
		DC->SetContactData(obj1,obj2,(real)m_Restitution);
		m_vContacts.push_back(DC);
	}
	else
	{
		DragonContact *DC = new DragonContact();
		real centerDist = obj1->GetPhysicsPointer()->GetNormal() * obj2->GetPhysicsPointer()->getPosition() - obj1->GetPhysicsPointer()->GetDistFromOrigin();
		DragonXVector3 normal = obj1->GetPhysicsPointer()->GetNormal();
		real penetration = -centerDist;
		if(centerDist < 0)
		{
			normal.invert();
			penetration = -penetration;
		}
		penetration+=obj2->GetPhysicsPointer()->GetRadius();
		// fill out contact data
		DC->m_vContactNormal=normal;
		DC->m_rPenetration=penetration;
		DC->m_vContactPoint = obj2->GetPhysicsPointer()->getPosition() - obj1->GetPhysicsPointer()->GetNormal() * centerDist;
		DC->SetContactData(obj2,obj1,(real)m_Restitution);
		m_vContacts.push_back(DC);
	}
}
void DragonContactRegistry::GenerateContactMeshSphere(BaseGameEntity* obj1,BaseGameEntity* obj2)
{
	if(obj1->GetPhysicsPointer()->mI_PrimitiveType==PT_Mesh)// if obj1 is the mesh
	{
		for(auto sector : obj1->GetPhysicsPointer()->GetSectors())
		{
			if(SphereSphereIntersectTest(obj1->GetPhysicsPointer()->getPointInWorldSpace(sector->GetPosition()),sector->GetRadius(),obj2->GetPhysicsPointer()->getPosition(),obj2->GetPhysicsPointer()->GetRadius()))
			{
				for(auto tri : sector->GetDragonTris())
				{
					GenerateContactSphereTri(obj1,obj2,tri);
				}
			}
		}
	}
	else// obj2 is mesh
	{
		for(auto sector : obj2->GetPhysicsPointer()->GetSectors())
		{
			if(SphereSphereIntersectTest(obj2->GetPhysicsPointer()->getPointInWorldSpace(sector->GetPosition()),sector->GetRadius(),obj1->GetPhysicsPointer()->getPosition(),obj1->GetPhysicsPointer()->GetRadius()))
			{
				for(auto tri : sector->GetDragonTris())
				{
					GenerateContactSphereTri(obj2,obj1,tri);
				}
			}
		}
	}
}
void DragonContactRegistry::GenerateContactSphereTri(BaseGameEntity* mesh,BaseGameEntity* sphere,DragonTriangle* tri)
{
	//DragonXVector3 p = ClosestPtPointTriangle(sphere->GetPhysicsPointer()->getPosition(),mesh->GetPhysicsPointer()->getPointInWorldSpace(mesh->GetPhysicsPointer()->getDirectionInLocalSpace(tri->GetVertA())),mesh->GetPhysicsPointer()->getPointInWorldSpace(mesh->GetPhysicsPointer()->getDirectionInLocalSpace(tri->GetVertB())),mesh->GetPhysicsPointer()->getPointInWorldSpace(mesh->GetPhysicsPointer()->getDirectionInLocalSpace(tri->GetVertC())));
	DragonXVector3 p = ClosestPtPointTriangle(sphere->GetPhysicsPointer()->getPosition(),mesh->GetPhysicsPointer()->getPointInWorldSpace(tri->GetVertA()),mesh->GetPhysicsPointer()->getPointInWorldSpace(tri->GetVertB()),mesh->GetPhysicsPointer()->getPointInWorldSpace(tri->GetVertC()));
	p-=sphere->GetPhysicsPointer()->getPosition();
	if (p*p<=sphere->GetPhysicsPointer()->GetRadius()*sphere->GetPhysicsPointer()->GetRadius())
	{
		DragonContact *DC = new DragonContact();
		real pen = sphere->GetPhysicsPointer()->GetRadius()-p.GetMagnitude();
		DC->m_rPenetration=pen;
		DC->m_vContactNormal=mesh->GetPhysicsPointer()->getDirectionInWorldSpace(mesh->GetPhysicsPointer()->getDirectionInLocalSpace(tri->GetNormal(mesh->GetPhysicsPointer()->getPointInWorldSpace(tri->GetVertA()),mesh->GetPhysicsPointer()->getPointInWorldSpace(tri->GetVertB()),mesh->GetPhysicsPointer()->getPointInWorldSpace(tri->GetVertC()))));
		//DC->m_vContactNormal=mesh->GetPhysicsPointer()->getDirectionInWorldSpace(tri->GetNormal());
		p+=sphere->GetPhysicsPointer()->getPosition();
		DC->m_vContactPoint=p;
		DC->SetContactData(mesh,sphere,(real)m_Restitution);
		m_vContacts.push_back(DC);
	}
}
// sphere obb
void DragonContactRegistry::GenerateContactSphereOBB(BaseGameEntity* obj1,BaseGameEntity* obj2)
{
	if(obj1->GetPhysicsPointer()->mI_PrimitiveType==PT_Sphere)//if object 1 is the sphere
	{
		DragonXVector3 center = obj1->GetPhysicsPointer()->getPosition();
		DragonXVector3 relCenter = obj2->GetPhysicsPointer()->getTransform().TransformInverse(center);
		DragonXVector3 closetsPt(0.0f,0.0f,0.0f);
		real dist;
		// clamp each coordinate to the box
		dist = relCenter.x;
		if(dist > obj2->GetPhysicsPointer()->GetHalfSizes().x)
		{
			dist = obj2->GetPhysicsPointer()->GetHalfSizes().x;
		}
		if(dist < -obj2->GetPhysicsPointer()->GetHalfSizes().x)
		{
			dist = -obj2->GetPhysicsPointer()->GetHalfSizes().x;
		}
		closetsPt.x = dist;

		dist = relCenter.y;
		if(dist > obj2->GetPhysicsPointer()->GetHalfSizes().y)
		{
			dist = obj2->GetPhysicsPointer()->GetHalfSizes().y;
		}
		if(dist < -obj2->GetPhysicsPointer()->GetHalfSizes().y)
		{
			dist = -obj2->GetPhysicsPointer()->GetHalfSizes().y;
		}
		closetsPt.y = dist;

		dist = relCenter.z;
		if(dist > obj2->GetPhysicsPointer()->GetHalfSizes().z)
		{
			dist = obj2->GetPhysicsPointer()->GetHalfSizes().z;
		}
		if(dist < -obj2->GetPhysicsPointer()->GetHalfSizes().z)
		{
			dist = -obj2->GetPhysicsPointer()->GetHalfSizes().z;
		}
		closetsPt.z = dist;

		//check we are in contact last change for out
		dist = (closetsPt - relCenter).GetMagSquared();

		if(dist > obj1->GetPhysicsPointer()->GetRadius() * obj1->GetPhysicsPointer()->GetRadius())
		{
			return;
		}
		else
		{
			DragonContact *DC = new DragonContact();
			DragonXVector3 closestPtWorld = obj2->GetPhysicsPointer()->getTransform().transform(closetsPt);
			DC->m_vContactNormal=(closestPtWorld - obj1->GetPhysicsPointer()->getPosition());
			DC->m_vContactNormal.Normalize();
			DC->m_vContactPoint=closestPtWorld;
			DC->m_rPenetration=obj1->GetPhysicsPointer()->GetRadius()-sqrt(dist);
			DC->SetContactData(obj2,obj1,(real)m_Restitution);
			m_vContacts.push_back(DC);
		}
	}
	else //obj2 is the sphere and obj1 is the obb
	{
		DragonXVector3 center = obj2->GetPhysicsPointer()->getPosition();
		DragonXVector3 relCenter = obj1->GetPhysicsPointer()->getTransform().TransformInverse(center);
		DragonXVector3 closetsPt(0.0f,0.0f,0.0f);
		real dist;
		// clamp each coordinate to the box
		dist = relCenter.x;
		if(dist > obj1->GetPhysicsPointer()->GetHalfSizes().x)
		{
			dist = obj1->GetPhysicsPointer()->GetHalfSizes().x;
		}
		if(dist < -obj1->GetPhysicsPointer()->GetHalfSizes().x)
		{
			dist = -obj1->GetPhysicsPointer()->GetHalfSizes().x;
		}
		closetsPt.x = dist;

		dist = relCenter.y;
		if(dist > obj1->GetPhysicsPointer()->GetHalfSizes().y)
		{
			dist = obj1->GetPhysicsPointer()->GetHalfSizes().y;
		}
		if(dist < -obj1->GetPhysicsPointer()->GetHalfSizes().y)
		{
			dist = -obj1->GetPhysicsPointer()->GetHalfSizes().y;
		}
		closetsPt.y = dist;

		dist = relCenter.z;
		if(dist > obj1->GetPhysicsPointer()->GetHalfSizes().z)
		{
			dist = obj1->GetPhysicsPointer()->GetHalfSizes().z;
		}
		if(dist < -obj1->GetPhysicsPointer()->GetHalfSizes().z)
		{
			dist = -obj1->GetPhysicsPointer()->GetHalfSizes().z;
		}
		closetsPt.z = dist;

		//check we are in contact last change for out
		dist = (closetsPt - relCenter).GetMagSquared();

		if(dist > obj2->GetPhysicsPointer()->GetRadius() * obj2->GetPhysicsPointer()->GetRadius())
		{
			return;
		}
		else
		{
			DragonContact *DC = new DragonContact();
			DragonXVector3 closestPtWorld = obj1->GetPhysicsPointer()->getTransform().transform(closetsPt);
			DC->m_vContactNormal=(closestPtWorld - obj2->GetPhysicsPointer()->getPosition());
			DC->m_vContactNormal.Normalize();
			DC->m_vContactPoint=closestPtWorld;
			DC->m_rPenetration=obj2->GetPhysicsPointer()->GetRadius()-sqrt(dist);
			DC->SetContactData(obj1,obj2,(real)m_Restitution);
			m_vContacts.push_back(DC);
		}
	}
}
// obb plane
void DragonContactRegistry::GenerateContactOBBPlane(BaseGameEntity* obj1,BaseGameEntity* obj2)
{
	// We have an intersection, so find the intersection points. We can make
	// do with only checking vertices's. If the box is resting on a plane
	// or on an edge, it will be reported as four or two contact points.

	// Go through each combination of + and - for each half-size
	static real mults[8][3] = {{1,1,1},{-1,1,1},{1,-1,1},{-1,-1,1},
	{1,1,-1},{-1,1,-1},{1,-1,-1},{-1,-1,-1}};

	if(obj1->GetPhysicsPointer()->mI_PrimitiveType==PT_OBB)//if obj1 is an obb
	{
		for(unsigned i = 0; i < 8; i++)
		{
			DragonXVector3 vertexPos(mults[i][0],mults[i][1],mults[i][2]);
			vertexPos.componentProductUpdate(obj1->GetPhysicsPointer()->GetHalfSizes());
			vertexPos = obj1->GetPhysicsPointer()->getTransform().transform(vertexPos);

			real vertexDist = vertexPos * obj2->GetPhysicsPointer()->GetNormal();

			if(vertexDist <= obj2->GetPhysicsPointer()->GetDistFromOrigin())
			{
				DragonContact *DC = new DragonContact();
				DC->m_vContactPoint=obj2->GetPhysicsPointer()->GetNormal();
				DC->m_vContactPoint*=(vertexDist-obj2->GetPhysicsPointer()->GetDistFromOrigin());
				DC->m_vContactPoint+=vertexPos;
				DC->m_vContactNormal = obj2->GetPhysicsPointer()->GetNormal();
				DC->m_rPenetration=obj2->GetPhysicsPointer()->GetDistFromOrigin()-vertexDist;
				DC->SetContactData(obj1,obj2,m_Restitution);
				m_vContacts.push_back(DC);
				/*if(m_vContacts.size()>=(unsigned)m_iLimit)
				{
				return;
				}*/
			}
		}
	}
	else//otherwise obj2 is the obb
	{
		for(unsigned i = 0; i < 8; i++)
		{
			DragonXVector3 vertexPos(mults[i][0],mults[i][1],mults[i][2]);
			vertexPos.componentProductUpdate(obj2->GetPhysicsPointer()->GetHalfSizes());
			vertexPos = obj2->GetPhysicsPointer()->getTransform().transform(vertexPos);

			real vertexDist = vertexPos * obj1->GetPhysicsPointer()->GetNormal();

			if(vertexDist <= obj1->GetPhysicsPointer()->GetDistFromOrigin())
			{
				DragonContact *DC = new DragonContact();
				DC->m_vContactPoint=obj1->GetPhysicsPointer()->GetNormal();
				DC->m_vContactPoint*=(vertexDist-obj1->GetPhysicsPointer()->GetDistFromOrigin());
				DC->m_vContactPoint+=vertexPos;
				DC->m_vContactNormal = obj1->GetPhysicsPointer()->GetNormal();
				DC->m_rPenetration=obj1->GetPhysicsPointer()->GetDistFromOrigin()-vertexDist;
				DC->SetContactData(obj2,obj1,m_Restitution);
				m_vContacts.push_back(DC);
				/*if(m_vContacts.size()>=(unsigned)m_iLimit)
				{
				return;
				}*/
			}
		}
	}
}
// obb obb
void DragonContactRegistry::GenerateContactOBBOBB(BaseGameEntity* obj1,BaseGameEntity* obj2)
{
	ObbObbContactGen(*obj1,*obj2);
}

void DragonContactRegistry::ClearPairs()
{
	for(const auto DragonicPair : m_vContactPairs)
	{
		delete DragonicPair;
	}
	m_vContactPairs.clear();
}

// we need a way to remove contacts
void DragonContactRegistry::RemoveAllContactsFrom(BaseGameEntity* obj)
{
	std::vector<DragonicPair*> keep;
	std::vector<DragonContact*> keepers;
	for(auto DragonicPair : m_vContactPairs)
	{
		if(DragonicPair->obj1==obj||DragonicPair->obj2==obj)
		{
			delete DragonicPair;
			DragonicPair=nullptr;
		}
		else
		{
			keep.push_back(DragonicPair);
		}
	}
	for(auto DragonContact : m_vContacts)
	{
		if(DragonContact->m_aEntitiesContacted[0]==obj||DragonContact->m_aEntitiesContacted[1]==obj)
		{
			delete DragonContact;
			DragonContact=nullptr;
		}
		else
		{
			keepers.push_back(DragonContact);
		}
	}
	m_vContactPairs.clear();
	m_vContactPairs=keep;
	m_vContacts.clear();
	m_vContacts=keepers;
}
// for ease of erasing we will make it possible to remove multiple
void DragonContactRegistry::RemoveContactsFromMultiple(std::vector<BaseGameEntity*>& list)
{
	for(const auto BaseGameEntity : list)
	{
		RemoveAllContactsFrom(BaseGameEntity);
	}
}

void DragonContactRegistry::GenerateContactMeshObb(BaseGameEntity* obj1,BaseGameEntity* obj2)
{
	if(obj1->GetPhysicsPointer()->mI_PrimitiveType==PT_Mesh)// if obj1 is the mesh
	{
		for(auto sector : obj1->GetPhysicsPointer()->GetSectors())
		{
			if(SphereSphereIntersectTest(obj1->GetPhysicsPointer()->getPointInWorldSpace(sector->GetPosition()),sector->GetRadius(),obj2->GetPhysicsPointer()->getPosition(),obj2->GetPhysicsPointer()->GetRadius()))
			{
				for(auto tri : sector->GetDragonTris())
				{
					GenerateContactObbTri(obj1,obj2,tri);
				}
			}
		}
	}
	else// obj2 is mesh
	{
		for(auto sector : obj2->GetPhysicsPointer()->GetSectors())
		{
			if(SphereSphereIntersectTest(obj2->GetPhysicsPointer()->getPointInWorldSpace(sector->GetPosition()),sector->GetRadius(),obj1->GetPhysicsPointer()->getPosition(),obj1->GetPhysicsPointer()->GetRadius()))
			{
				for(auto tri : sector->GetDragonTris())
				{
					GenerateContactObbTri(obj2,obj1,tri);
				}
			}
		}
	}
}

static inline real getMax(real zed, real ein, real zwei)
{
	real ret=-9999999999.9f;
	if(zed>ret)
	{
		ret=zed;
	}
	if(ein>ret)
	{
		ret = ein;
	}
	if(zwei>ret)
	{
		ret = zwei;
	}
	return ret;
}

static inline real getMinPos(real zed, real ein, real zwei)
{
	real ret=9999999999.9f;
	if(zed<ret&&zed>=0.0f)
	{
		ret=zed;
	}
	if(ein<ret&&ein>=0.0f)
	{
		ret = ein;
	}
	if(zwei<ret&&zwei>=0.0f)
	{
		ret = zwei;
	}
	return ret;
}

static inline real getMin(real zed, real ein, real zwei)
{
	real ret=9999999999.9f;
	if(zed<ret)
	{
		ret=zed;
	}
	if(ein<ret)
	{
		ret = ein;
	}
	if(zwei<ret)
	{
		ret = zwei;
	}
	return ret;
}

static inline real getMax(real zed, real ein)
{
	if(zed>ein)
	{
		return zed;
	}
	else
	{
		return ein;
	}
}

static inline real getMin(real zed, real ein)
{
	if(zed<ein)
	{
		return zed;
	}
	else
	{
		return ein;
	}
}

inline DragonXVector3 CalcObbEdgeContactpoint(DragonXVector3& p,DragonXVector3& d,BaseGameEntity* obb,real length)
{
	d.Normalize();
	real EPSILON = 0.001f;
	DragonXVector3 retPoint;
	real tmin=-999999999.9f;
	real tmax=length;
	if(abs(d.x)>EPSILON)
	{
		real ood = 1.0f / d.x;
		real t1 = (-obb->GetPhysicsPointer()->GetHalfSizes().x - p.x)*ood;
		real t2 = (obb->GetPhysicsPointer()->GetHalfSizes().x - p.x)*ood;
		if(t1>t2)
		{
			real temp = t1;
			t1=t2;
			t2=temp;
		}
		if(t1>tmin)
		{
			tmin=t1;
		}
		if (t2>tmax)
		{
			tmax=t2;
		}
	}

	if(abs(d.y)>EPSILON)
	{
		real ood = 1.0f / d.y;
		real t1 = (-obb->GetPhysicsPointer()->GetHalfSizes().y - p.y)*ood;
		real t2 = (obb->GetPhysicsPointer()->GetHalfSizes().y - p.y)*ood;
		if(t1>t2)
		{
			real temp = t1;
			t1=t2;
			t2=temp;
		}
		if(t1>tmin)
		{
			tmin=t1;
		}
		if (t2>tmax)
		{
			tmax=t2;
		}
	}

	if(abs(d.z)>EPSILON)
	{
		real ood = 1.0f / d.z;
		real t1 = (-obb->GetPhysicsPointer()->GetHalfSizes().z - p.z)*ood;
		real t2 = (obb->GetPhysicsPointer()->GetHalfSizes().z - p.z)*ood;
		if(t1>t2)
		{
			real temp = t1;
			t1=t2;
			t2=temp;
		}
		if(t1>tmin)
		{
			tmin=t1;
		}
		if (t2>tmax)
		{
			tmax=t2;
		}
	}
	retPoint = p+d*tmin;
	return retPoint;
}

// obb tri
void DragonContactRegistry::GenerateContactObbTri(BaseGameEntity* mesh,BaseGameEntity* obb,DragonTriangle* tri)
{
	DragonXVector3 vertA = obb->GetPhysicsPointer()->getPointInLocalSpace(mesh->GetPhysicsPointer()->getPointInWorldSpace(tri->GetVertA()));
	DragonXVector3 vertB = obb->GetPhysicsPointer()->getPointInLocalSpace(mesh->GetPhysicsPointer()->getPointInWorldSpace(tri->GetVertB()));
	DragonXVector3 vertC = obb->GetPhysicsPointer()->getPointInLocalSpace(mesh->GetPhysicsPointer()->getPointInWorldSpace(tri->GetVertC()));
	DragonXVector3 norm = tri->GetNormal(vertA,vertB,vertC);

	DragonXVector3 f0=vertB-vertA;

	DragonXVector3 f1=vertC-vertB;

	DragonXVector3 f2=vertA-vertC;

	DragonXVector3 tempConPoint;
	int axis = -1;
	DragonXVector3 bestAxis;
	real pen=999999999.0f;

	// one test against the tri face normal   this is the box plane test finish later
	DragonXVector3 axisfn = norm;
	bool triNormInverted=false;
	axisfn.Normalize();
	real p0 = vertA*axisfn;
	real p1 = vertB*axisfn;
	real p2 = vertC*axisfn;
	if(getMin(p0,p1,p2)<0.0f)
	{
		triNormInverted=true;
		norm.invert();
		axisfn.invert();
		p0 = vertA*axisfn;
		p1 = vertB*axisfn;
		p2 = vertC*axisfn;
	}
	real tpen = transformToAxis(*obb,axisfn)-getMin(p0,p1);
	if(getMax(-1.0f*getMax(p0,p1,p2),getMin(p0,p1,p2))<=transformToAxis(*obb,axisfn))
	{
		if(tpen<pen&&tpen>=0.0f)
		{
			axis=0;
			bestAxis=axisfn;
			pen=tpen;
		}
	}
	else
	{
		return;
	}

	bool inverted = false;

	// 3 obb face normals
	DragonXVector3 axis0 = obb->GetPhysicsPointer()->getTransform().getAxisVector(0);
	axis0.Normalize();
	p0 = vertA*axis0;
	p1 = vertB*axis0;
	p2 = vertC*axis0;
	if(getMin(p0,p1,p2)<0.0f)
	{
		inverted=true;
		axis0.invert();
		p0 = vertA*axis0;
		p1 = vertB*axis0;
		p2 = vertC*axis0;
	}
	tpen = transformToAxis(*obb,axis0)-getMin(p0,p1,p2);
	if(getMax(-1.0f*getMax(p0,p1,p2),getMin(p0,p1,p2))<=transformToAxis(*obb,axis0))
	{
		if(tpen<pen&&tpen>=0.0f)
		{
			if(tpen==transformToAxis(*obb,axis0)-p0)
			{
				tempConPoint=vertA;
			}
			if(tpen==transformToAxis(*obb,axis0)-p1)
			{
				tempConPoint=vertB;
			}
			if(tpen==transformToAxis(*obb,axis0)-p2)
			{
				tempConPoint=vertC;
			}
			axis=1;
			bestAxis=axis0;
			pen=tpen;
		}
	}
	else
	{
		return;
	}

	inverted=false;
	DragonXVector3 axis1 = obb->GetPhysicsPointer()->getTransform().getAxisVector(1);
	axis1.Normalize();
	p0 = vertA*axis1;
	p1 = vertB*axis1;
	p2 = vertC*axis1;
	if(getMin(p0,p1,p2)<0.0f)
	{
		inverted=true;
		axis1.invert();
		p0 = vertA*axis1;
		p1 = vertB*axis1;
		p2 = vertC*axis1;
	}
	tpen = transformToAxis(*obb,axis1)-getMin(p0,p1,p2);
	if(getMax(-1.0f*getMax(p0,p1,p2),getMin(p0,p1,p2))<=transformToAxis(*obb,axis1))
	{
		if(tpen<pen&&tpen>=0.0f)
		{
			if(tpen==transformToAxis(*obb,axis1)-p0)
			{
				tempConPoint=vertA;
			}
			if(tpen==transformToAxis(*obb,axis1)-p1)
			{
				tempConPoint=vertB;
			}
			if(tpen==transformToAxis(*obb,axis1)-p2)
			{
				tempConPoint=vertC;
			}
			axis=2;
			bestAxis=axis1;
			pen=tpen;
		}
	}
	else
	{
		return;
	}

	inverted=false;
	DragonXVector3 axis2 = obb->GetPhysicsPointer()->getTransform().getAxisVector(2);
	axis2.Normalize();
	p0 = vertA*axis2;
	p1 = vertB*axis2;
	p2 = vertC*axis2;
	if(getMin(p0,p1,p2)<0.0f)
	{
		inverted=true;
		axis2.invert();
		p0 = vertA*axis2;
		p1 = vertB*axis2;
		p2 = vertC*axis2;
	}
	tpen = transformToAxis(*obb,axis2)-getMin(p0,p1,p2);
	if(getMax(-1.0f*getMax(p0,p1,p2),getMin(p0,p1,p2))<=transformToAxis(*obb,axis2))
	{
		if(tpen<pen&&tpen>=0.0f)
		{
			if(tpen==transformToAxis(*obb,axis2)-p0)
			{
				tempConPoint=vertA;
			}
			if(tpen==transformToAxis(*obb,axis2)-p1)
			{
				tempConPoint=vertB;
			}
			if(tpen==transformToAxis(*obb,axis2)-p2)
			{
				tempConPoint=vertC;
			}
			axis=3;
			bestAxis=axis2;
			pen=tpen;
		}
	}
	else
	{
		return;
	}

	inverted=false;
	// cross product axis tests
	DragonXVector3 axis00 = obb->GetPhysicsPointer()->getTransform().getAxisVector(0)%f0;
	axis00.Normalize();
	if(axis00.GetMagSquared()>0.0001)
	{
		p0 = vertA*axis00;
		p1 = vertB*axis00;
		p2 = vertC*axis00;
		if(getMin(p0,p1,p2)<0.0f)
		{
			inverted=true;
			axis00.invert();
			p0 = vertA*axis00;
			p1 = vertB*axis00;
			p2 = vertC*axis00;
		}
		tpen = transformToAxis(*obb,axis00)-getMin(p0,p2);
		if(getMax(-1.0f*getMax(p0,p2),getMin(p0,p2))<=transformToAxis(*obb,axis00))
		{
			if(tpen<pen&&tpen>=0.0f)
			{
				axis=4;
				if(inverted)
				{
					//axis00.invert();
					inverted=false;
				}
				bestAxis=axis00;
				pen=tpen;
			}
		}
		else
		{
			return;
		}
	}

	DragonXVector3 axis01 = obb->GetPhysicsPointer()->getTransform().getAxisVector(0)%f1;
	axis01.Normalize();
	if(axis01.GetMagSquared()>0.0001)
	{
		p0 = vertA*axis01;
		p1 = vertB*axis01;
		p2 = vertC*axis01;
		if(getMin(p0,p1,p2)<0.0f)
		{
			inverted=true;
			axis01.invert();
			p0 = vertA*axis01;
			p1 = vertB*axis01;
			p2 = vertC*axis01;
		}
		tpen = transformToAxis(*obb,axis01)-getMin(p0,p2);
		if(getMax(-1.0f*getMax(p0,p2),getMin(p0,p2))<=transformToAxis(*obb,axis01))
		{
			if(tpen<pen&&tpen>=0.0f)
			{
				axis=5;
				if(inverted)
				{
					//axis01.invert();
					inverted=false;
				}
				bestAxis=axis01;
				pen=tpen;
			}
		}
		else
		{
			return;
		}
	}

	DragonXVector3 axis02 = obb->GetPhysicsPointer()->getTransform().getAxisVector(0)%f2;
	axis02.Normalize();
	if(axis02.GetMagSquared()>0.0001)
	{
		p0 = vertA*axis02;
		p1 = vertB*axis02;
		p2 = vertC*axis02;
		if(getMin(p0,p1,p2)<0.0f)
		{
			inverted=true;
			axis02.invert();
			p0 = vertA*axis02;
			p1 = vertB*axis02;
			p2 = vertC*axis02;
		}
		tpen = transformToAxis(*obb,axis02)-getMin(p1,p2);
		if(getMax(-1.0f*getMax(p1,p2),getMin(p1,p2))<=transformToAxis(*obb,axis02))
		{
			if(tpen<pen&&tpen>=0.0f)
			{
				axis=6;
				if(inverted)
				{
					//axis02.invert();
					inverted=false;
				}
				bestAxis=axis02;
				pen=tpen;
			}
		}
		else
		{
			return;
		}
	}

	DragonXVector3 axis10 = obb->GetPhysicsPointer()->getTransform().getAxisVector(1)%f0;
	axis10.Normalize();
	if(axis10.GetMagSquared()>0.0001)
	{
		p0 = vertA*axis10;
		p1 = vertB*axis10;
		p2 = vertC*axis10;
		if(getMin(p0,p1,p2)<0.0f)
		{
			inverted=true;
			axis10.invert();
			p0 = vertA*axis10;
			p1 = vertB*axis10;
			p2 = vertC*axis10;
		}
		tpen=transformToAxis(*obb,axis10)-getMin(p0,p2);
		if(getMax(-1.0f*getMax(p0,p2),getMin(p0,p2))<=transformToAxis(*obb,axis10))
		{
			if(tpen<pen&&tpen>=0.0f)
			{
				axis=7;
				if(inverted)
				{
					//axis10.invert();
					inverted=false;
				}
				bestAxis=axis10;
				pen=tpen;
			}
		}
		else
		{
			return;
		}
	}

	DragonXVector3 axis11 = obb->GetPhysicsPointer()->getTransform().getAxisVector(1)%f1;
	axis11.Normalize();
	if(axis11.GetMagSquared()>0.0001)
	{
		p0 = vertA*axis11;
		p1 = vertB*axis11;
		p2 = vertC*axis11;
		if(getMin(p0,p1,p2)<0.0f)
		{
			inverted=true;
			axis11.invert();
			p0 = vertA*axis11;
			p1 = vertB*axis11;
			p2 = vertC*axis11;
		}
		tpen=transformToAxis(*obb,axis11)-getMin(p0,p2);
		if(getMax(-1.0f*getMax(p0,p2),getMin(p0,p2))<=transformToAxis(*obb,axis11))
		{
			if(tpen<pen&&tpen>=0.0f)
			{
				axis=8;
				if(inverted)
				{
					//axis11.invert();
					inverted=false;
				}
				bestAxis=axis11;
				pen=tpen;
			}
		}
		else
		{
			return;
		}
	}

	DragonXVector3 axis12 = obb->GetPhysicsPointer()->getTransform().getAxisVector(1)%f2;
	axis12.Normalize();
	if(axis12.GetMagSquared()>0.0001)
	{
		p0 = vertA*axis12;
		p1 = vertB*axis12;
		p2 = vertC*axis12;
		if(getMin(p0,p1,p2)<0.0f)
		{
			inverted=true;
			axis12.invert();
			p0 = vertA*axis12;
			p1 = vertB*axis12;
			p2 = vertC*axis12;
		}
		tpen = transformToAxis(*obb,axis12)-getMin(p0,p1);
		if(getMax(-1.0f*getMax(p0,p1),getMin(p0,p1))<=transformToAxis(*obb,axis12))
		{
			if(tpen<pen&&tpen>=0.0f)
			{
				axis=9;
				if(inverted)
				{
					//axis12.invert();
					inverted=false;
				}
				bestAxis=axis12;
				pen=tpen;
			}
		}
		else
		{
			return;
		}
	}

	DragonXVector3 axis20 = obb->GetPhysicsPointer()->getTransform().getAxisVector(2)%f0;
	axis20.Normalize();
	if(axis20.GetMagSquared()>0.0001)
	{
		p0 = vertA*axis20;
		p1 = vertB*axis20;
		p2 = vertC*axis20;
		if(getMin(p0,p1,p2)<0.0f)
		{
			inverted=true;
			axis20.invert();
			p0 = vertA*axis20;
			p1 = vertB*axis20;
			p2 = vertC*axis20;
		}
		tpen=transformToAxis(*obb,axis20)-getMin(p0,p2);
		if(getMax(-1.0f*getMax(p0,p2),getMin(p0,p2))<=transformToAxis(*obb,axis20))
		{
			if(tpen<pen&&tpen>=0.0f)
			{
				axis=10;
				if(inverted)
				{
					//axis20.invert();
					inverted=false;
				}
				bestAxis=axis20;
				pen=tpen;
			}
		}
		else
		{
			return;
		}
	}

	DragonXVector3 axis21 = obb->GetPhysicsPointer()->getTransform().getAxisVector(2)%f1;
	axis21.Normalize();
	if(axis21.GetMagSquared()>0.0001)
	{
		p0 = vertA*axis21;
		p1 = vertB*axis21;
		p2 = vertC*axis21;
		if(getMin(p0,p1,p2)<0.0f)
		{
			inverted=true;
			axis21.invert();
			p0 = vertA*axis21;
			p1 = vertB*axis21;
			p2 = vertC*axis21;
		}
		tpen=transformToAxis(*obb,axis21)-getMin(p0,p1);
		if(getMax(-1.0f*getMax(p0,p1),getMin(p0,p1))<=transformToAxis(*obb,axis21))
		{
			if(tpen<pen&&tpen>=0.0f)
			{
				axis=11;
				if(inverted)
				{
					//axis21.invert();
					inverted=false;
				}
				bestAxis=axis21;
				pen=tpen;
			}
		}
		else
		{
			return;
		}
	}

	DragonXVector3 axis22 = obb->GetPhysicsPointer()->getTransform().getAxisVector(2)%f2;
	axis22.Normalize();
	if(axis22.GetMagSquared()>0.0001)
	{
		p0 = vertA*axis22;
		p1 = vertB*axis22;
		p2 = vertC*axis22;
		if(getMin(p0,p1,p2)<0.0f)
		{
			inverted=true;
			axis22.invert();
			p0 = vertA*axis22;
			p1 = vertB*axis22;
			p2 = vertC*axis22;
		}
		tpen=transformToAxis(*obb,axis22)-getMin(p1,p2);
		if(getMax(-1.0f*getMax(p1,p2),getMin(p1,p2))<=transformToAxis(*obb,axis22))
		{
			if(tpen<pen&&tpen>=0.0f)
			{
				axis=12;
				if(inverted)
				{
					//axis22.invert();
					inverted=false;
				}
				bestAxis=axis22;
				pen=tpen;
			}
		}
		else
		{
			return;
		}
	}

	if(axis!=-1)
	{
		if(axis==0)
		{
			DragonXVector3 Wnorm = tri->GetNormal(mesh->GetPhysicsPointer()->getPointInWorldSpace(tri->GetVertA()),mesh->GetPhysicsPointer()->getPointInWorldSpace(tri->GetVertB()),mesh->GetPhysicsPointer()->getPointInWorldSpace(tri->GetVertC()));
			// Go through each combination of + and - for each half-size
			static real mults[8][3] = {{1,1,1},{-1,1,1},{1,-1,1},{-1,-1,1},
			{1,1,-1},{-1,1,-1},{1,-1,-1},{-1,-1,-1}};
			DragonXVector3 pcp;
			for(unsigned i = 0; i < 8; i++)
			{
				DragonXVector3 vertexPos(mults[i][0],mults[i][1],mults[i][2]);
				vertexPos.componentProductUpdate(obb->GetPhysicsPointer()->GetHalfSizes());
				//vertexPos = obb->GetPhysicsPointer()->getTransform().transform(vertexPos);
				DragonXVector3 p = ClosestPtPointTriangle(vertexPos,vertA,vertB,vertC);
				clostestPtPointOBBOnObbNotIn(obb->GetPhysicsPointer()->getPointInWorldSpace(p),obb,pcp);
				real pdist= p*norm;
				p=obb->GetPhysicsPointer()->getPointInLocalSpace(pcp);
				real vdist = vertexPos*norm;
				if(vdist>=pdist)
				{
					DragonContact *DC = new DragonContact();
					DC->m_vContactPoint=Wnorm;
					DC->m_vContactPoint*=pen;
					DC->m_vContactPoint+=pcp;
					//DC->m_vContactNormal = obb->GetPhysicsPointer()->getDirectionInWorldSpace(norm);
					DC->m_vContactNormal = Wnorm;
					if(triNormInverted)
					{
						DC->m_vContactNormal.invert();
					}
					DC->m_rPenetration=(pen);
					DC->SetContactData(mesh,obb,m_Restitution);
					m_vContacts.push_back(DC);
				}
			}
			return;
		}
		if(axis>0&&axis<4)
		{
			DragonContact *DC = new DragonContact();
			DC->m_vContactPoint=obb->GetPhysicsPointer()->getPointInWorldSpace(tempConPoint);
			DC->m_vContactNormal = obb->GetPhysicsPointer()->getDirectionInWorldSpace(bestAxis);
			DC->m_vContactNormal.Normalize();
			DC->m_rPenetration=pen;
			DC->SetContactData(mesh,obb,m_Restitution);
			m_vContacts.push_back(DC);
			return;
		}
		DragonXVector3 cp;
		if(axis==4||axis==7||axis==10)
		{
			cp = CalcObbEdgeContactpoint(vertA,f0,obb,f0.GetMagnitude());
			DragonContact *DC = new DragonContact();
			DC->m_vContactPoint=obb->GetPhysicsPointer()->getPointInWorldSpace(cp);
			DC->m_vContactNormal = obb->GetPhysicsPointer()->getDirectionInWorldSpace(bestAxis);
			DC->m_vContactNormal.Normalize();
			DC->m_rPenetration=pen;
			DC->SetContactData(mesh,obb,m_Restitution);
			m_vContacts.push_back(DC);
			return;
		}

		if(axis==5||axis==8||axis==11)
		{
			cp = CalcObbEdgeContactpoint(vertB,f1,obb,f1.GetMagnitude());
			DragonContact *DC = new DragonContact();
			DC->m_vContactPoint=obb->GetPhysicsPointer()->getPointInWorldSpace(cp);
			DC->m_vContactNormal = obb->GetPhysicsPointer()->getDirectionInWorldSpace(bestAxis);
			DC->m_vContactNormal.Normalize();
			DC->m_rPenetration=pen;
			DC->SetContactData(mesh,obb,m_Restitution);
			m_vContacts.push_back(DC);
			return;
		}

		if(axis==6||axis==9||axis==12)
		{
			cp = CalcObbEdgeContactpoint(vertC,f2,obb,f2.GetMagnitude());
			DragonContact *DC = new DragonContact();
			DC->m_vContactPoint=obb->GetPhysicsPointer()->getPointInWorldSpace(cp);
			DC->m_vContactNormal = obb->GetPhysicsPointer()->getDirectionInWorldSpace(bestAxis);
			DC->m_vContactNormal.Normalize();
			DC->m_rPenetration=pen;
			DC->SetContactData(mesh,obb,m_Restitution);
			m_vContacts.push_back(DC);
			return;
		}
		return;
	}
}

void DragonContactRegistry::GenerateContactsMeshMesh(BaseGameEntity* obj1,BaseGameEntity* obj2)
{
	for(auto obj1Sector : obj1->GetPhysicsPointer()->GetSectors())
	{
		for(auto obj2Sector : obj2->GetPhysicsPointer()->GetSectors())
		{
			if(SphereSphereIntersectTest(obj1->GetPhysicsPointer()->getPointInWorldSpace(obj1Sector->GetPosition()),obj1Sector->GetRadius(),obj2->GetPhysicsPointer()->getPointInWorldSpace(obj2Sector->GetPosition()),obj2Sector->GetRadius()))
			{
				for(auto tri1 : obj1Sector->GetDragonTris())
				{
					for(auto tri2 : obj2Sector->GetDragonTris())
					{
						GenerateContactsTriTri(obj1,tri1,obj2,tri2);
					}
				}
			}
		}
	}
}
// gen contacts tri tri
void DragonContactRegistry::GenerateContactsTriTri(BaseGameEntity* mesh1,DragonTriangle* tri1,BaseGameEntity* mesh2,DragonTriangle* tri2)
{
	DragonXVector3 tri1Norm = tri1->GetNormal(mesh1->GetPhysicsPointer()->getPointInWorldSpace(tri1->GetVertA()),mesh1->GetPhysicsPointer()->getPointInWorldSpace(tri1->GetVertB()),mesh1->GetPhysicsPointer()->getPointInWorldSpace(tri1->GetVertC()));
	DragonXVector3 tri2Norm = tri2->GetNormal(mesh2->GetPhysicsPointer()->getPointInWorldSpace(tri2->GetVertA()),mesh2->GetPhysicsPointer()->getPointInWorldSpace(tri2->GetVertB()),mesh2->GetPhysicsPointer()->getPointInWorldSpace(tri2->GetVertC()));
	real du0=0.0f;
	real du1=0.0f;
	real du2=0.0f;
	real dv0=0.0f;
	real dv1=0.0f;
	real dv2 = 0.0f;
	DragonXVector3 pointOfContact;
	real pen=999999999999.9f;
	DragonXVector3 contactNorm;

	if(!TestTriVertsAgainstPlane(mesh1->GetPhysicsPointer()->getPointInWorldSpace(tri1->GetVertA()),mesh1->GetPhysicsPointer()->getPointInWorldSpace(tri1->GetVertB()),mesh1->GetPhysicsPointer()->getPointInWorldSpace(tri1->GetVertC()),tri2Norm,tri2Norm*mesh2->GetPhysicsPointer()->getPointInWorldSpace(tri2->GetVertC()),du0,du1,du2))
	{
		return;
	}
	//pen depths of each vert of tri1 into tri 2 0 corespends to verta 1 to b...
	real du0du1=du0*du1;
	real du0du2=du0*du2;
	/*
	if(du0du1>0.0f&&du0du2>0.0f)
	{
	return;
	}
	*/
	if(!TestTriVertsAgainstPlane(mesh2->GetPhysicsPointer()->getPointInWorldSpace(tri2->GetVertA()),mesh2->GetPhysicsPointer()->getPointInWorldSpace(tri2->GetVertB()),mesh2->GetPhysicsPointer()->getPointInWorldSpace(tri2->GetVertC()),tri1Norm,tri1Norm*mesh1->GetPhysicsPointer()->getPointInWorldSpace(tri1->GetVertC()),dv0,dv1,dv2))
	{
		return;
	}
	//// same here just switched
	real dv0dv1=dv0*dv1;
	real dv0dv2=dv0*dv2;

	//if(dv0dv1>0.0f&&dv0dv2>0.0f)
	//{
	//	return;
	//}

	DragonXVector3 d = tri1Norm%tri2Norm;
	real max = (real)abs(d[0]);
	int index = 0;
	real bb = (real)abs(d[1]);
	real cc = (real)abs(d[2]);
	if(bb>max)
	{
		max=bb;
		index=1;
	}

	if(cc>max)
	{
		max=cc;
		index=2;
	}

	real vp0 = mesh1->GetPhysicsPointer()->getPointInWorldSpace(tri1->GetVertA())[index];
	real vp1 = mesh1->GetPhysicsPointer()->getPointInWorldSpace(tri1->GetVertB())[index];
	real vp2 = mesh1->GetPhysicsPointer()->getPointInWorldSpace(tri1->GetVertC())[index];

	real up0 = mesh2->GetPhysicsPointer()->getPointInWorldSpace(tri2->GetVertA())[index];
	real up1 = mesh2->GetPhysicsPointer()->getPointInWorldSpace(tri2->GetVertB())[index];
	real up2 = mesh2->GetPhysicsPointer()->getPointInWorldSpace(tri2->GetVertC())[index];

	real a,b,c,x0,x1;
	if(ComputeIntervals(vp0,vp1,vp2,dv0,dv1,dv2,dv0dv1,dv0dv1,a,b,c,x0,x1))
	{
		if(CoplanarTriTest(tri1Norm,mesh1->GetPhysicsPointer()->getPointInWorldSpace(tri1->GetVertA()),mesh1->GetPhysicsPointer()->getPointInWorldSpace(tri1->GetVertB()),mesh1->GetPhysicsPointer()->getPointInWorldSpace(tri1->GetVertC()),mesh2->GetPhysicsPointer()->getPointInWorldSpace(tri2->GetVertA()),mesh2->GetPhysicsPointer()->getPointInWorldSpace(tri2->GetVertB()),mesh2->GetPhysicsPointer()->getPointInWorldSpace(tri2->GetVertC())))
		{
			DragonContact *DC = new DragonContact();
			DC->m_vContactPoint=mesh1->GetPhysicsPointer()->getPointInWorldSpace(tri1->GetVertA());
			DC->m_vContactNormal = tri1Norm;
			//DC->m_vContactNormal.Normalize();
			DC->m_rPenetration=mesh1->GetPhysicsPointer()->getPointInWorldSpace(tri1->GetVertA())*tri1Norm-mesh2->GetPhysicsPointer()->getPointInWorldSpace(tri2->GetVertA())*tri1Norm;
			DC->SetContactData(mesh1,mesh2,m_Restitution);
			m_vContacts.push_back(DC);
			return;// the 2 tris over lap completely on the same plane contact gen is any vert normal of either tri 0 pen
		}
	}

	real D,e,f,y0,y1;
	if(ComputeIntervals(up0,up1,up2,du0,du1,du2,du0du1,du0du2,D,e,f,y0,y1))
	{
		if(CoplanarTriTest(tri1Norm,mesh1->GetPhysicsPointer()->getPointInWorldSpace(tri1->GetVertA()),mesh1->GetPhysicsPointer()->getPointInWorldSpace(tri1->GetVertB()),mesh1->GetPhysicsPointer()->getPointInWorldSpace(tri1->GetVertC()),mesh2->GetPhysicsPointer()->getPointInWorldSpace(tri2->GetVertA()),mesh2->GetPhysicsPointer()->getPointInWorldSpace(tri2->GetVertB()),mesh2->GetPhysicsPointer()->getPointInWorldSpace(tri2->GetVertC())))
		{
			DragonContact *DC = new DragonContact();
			DC->m_vContactPoint=mesh2->GetPhysicsPointer()->getPointInWorldSpace(tri2->GetVertA());
			DC->m_vContactNormal = tri2Norm;
			//DC->m_vContactNormal.Normalize();
			DC->m_rPenetration=mesh2->GetPhysicsPointer()->getPointInWorldSpace(tri2->GetVertA())*tri2Norm-mesh1->GetPhysicsPointer()->getPointInWorldSpace(tri1->GetVertA())*tri2Norm;
			DC->SetContactData(mesh2,mesh1,m_Restitution);
			m_vContacts.push_back(DC);
			return;// the 2 tris over lap completely on the same plane contact gen is any vert normal of either tri 0 pen
		}
	}

	//create local temp verts 4 each triangle in world space
	// tri 1
	DragonXVector3 tri1A = mesh1->GetPhysicsPointer()->getPointInWorldSpace(tri1->GetVertA());
	DragonXVector3 tri1B = mesh1->GetPhysicsPointer()->getPointInWorldSpace(tri1->GetVertB());
	DragonXVector3 tri1C = mesh1->GetPhysicsPointer()->getPointInWorldSpace(tri1->GetVertC());
	//tri2
	DragonXVector3 tri2A = mesh2->GetPhysicsPointer()->getPointInWorldSpace(tri2->GetVertA());
	DragonXVector3 tri2B = mesh2->GetPhysicsPointer()->getPointInWorldSpace(tri2->GetVertB());
	DragonXVector3 tri2C = mesh2->GetPhysicsPointer()->getPointInWorldSpace(tri2->GetVertC());

	// now compute the edges
	DragonXVector3 tri1ba = tri1B-tri1A;
	DragonXVector3 tri1cb = tri1C-tri1B;
	DragonXVector3 tri1ac = tri1A-tri1C;
	//tri2
	DragonXVector3 tri2ba = tri2B-tri2A;
	DragonXVector3 tri2cb = tri2C-tri2B;
	DragonXVector3 tri2ac = tri2A-tri2C;

	real tpen = 99999999999.9f;
	real pu0,pu1,pu2,pv0,pv1,pv2;
	int axis = -1;
	DragonXVector3 cnAxis;
	DragonXVector3 cp;
	if(du0<pen&&du0>0.0f)
	{
		pen = du0;
		axis = 0;
		cnAxis=tri2Norm;
		cp=tri1A;
	}
	if(du1<pen&&du1>0.0f)
	{
		pen = du1;
		axis = 1;
		cnAxis=tri2Norm;
		cp=tri1B;
	}
	if(du2<pen&&du2>0.0f)
	{
		pen = du2;
		axis = 2;
		cnAxis=tri2Norm;
		cp=tri1C;
	}
	// test tri2 verts against tri 1 plane
	if(dv0<pen&&dv0>0.0f)
	{
		pen = dv0;
		axis = 3;
		cnAxis=tri1Norm;
		cp=tri2A;
	}
	if(dv1<pen&&dv1>0.0f)
	{
		pen = dv1;
		axis = 4;
		cnAxis=tri1Norm;
		cp=tri2B;
	}
	if(dv2<pen&&dv2>0.0f)
	{
		pen = dv2;
		axis = 5;
		cnAxis=tri1Norm;
		cp=tri2C;
	}
	// 9 cross product axis tests
	// set 1
	DragonXVector3 testAxis = tri1ba%tri2ba;
	testAxis.Normalize();
	if(testAxis.GetMagSquared()>0.0001)
	{
		pu0=tri1A*testAxis;
		pu1=tri1B*testAxis;
		pu2=tri1C*testAxis;

		if(getMin(pu0,pu1,pu2)<0.0f)
		{
			testAxis.invert();
			pu0=tri1A*testAxis;
			pu1=tri1B*testAxis;
			pu2=tri1C*testAxis;
		}
		pv0=tri2A*testAxis;
		pv1=tri2B*testAxis;
		pv2=tri2C*testAxis;

		if(getMax(pv0,pv1,pv2)>getMin(pu0,pu1,pu2))
		{
			tpen=getMax(pv0,pv1,pv2)-getMin(pu0,pu1,pu2);
			if(tpen<pen&&tpen>0.0f)
			{
				pen=tpen;
				axis=6;
				cnAxis=testAxis;
			}
		}
		else
		{
			return;
		}
		if(getMin(pv0,pv1,pv2)<getMax(pu0,pu1,pu2))
		{
			tpen=getMax(pu0,pu1,pu2)-getMin(pv0,pv1,pv2);
			if(tpen<pen&&tpen>0.0f)
			{
				// note may need 2 invert contact normal
				pen=tpen;
				axis=7;
				cnAxis=testAxis;
			}
		}
		else
		{
			return;
		}
	}

	testAxis = tri1ba%tri2cb;
	testAxis.Normalize();
	if(testAxis.GetMagSquared()>0.0001)
	{
		pu0=tri1A*testAxis;
		pu1=tri1B*testAxis;
		pu2=tri1C*testAxis;

		if(getMin(pu0,pu1,pu2)<0.0f)
		{
			testAxis.invert();
			pu0=tri1A*testAxis;
			pu1=tri1B*testAxis;
			pu2=tri1C*testAxis;
		}
		pv0=tri2A*testAxis;
		pv1=tri2B*testAxis;
		pv2=tri2C*testAxis;

		if(getMax(pv0,pv1,pv2)>getMin(pu0,pu1,pu2))
		{
			tpen=getMax(pv0,pv1,pv2)-getMin(pu0,pu1,pu2);
			if(tpen<pen&&tpen>0.0f)
			{
				pen=tpen;
				axis=8;
				cnAxis=testAxis;
			}
		}
		else
		{
			return;
		}
		if(getMin(pv0,pv1,pv2)<getMax(pu0,pu1,pu2))
		{
			tpen=getMax(pu0,pu1,pu2)-getMin(pv0,pv1,pv2);
			if(tpen<pen&&tpen>0.0f)
			{
				// note may need 2 invert contact normal
				pen=tpen;
				axis=9;
				cnAxis=testAxis;
			}
		}
		else
		{
			return;
		}
	}

	testAxis = tri1ba%tri2ac;
	testAxis.Normalize();
	if(testAxis.GetMagSquared()>0.0001)
	{
		pu0=tri1A*testAxis;
		pu1=tri1B*testAxis;
		pu2=tri1C*testAxis;

		if(getMin(pu0,pu1,pu2)<0.0f)
		{
			testAxis.invert();
			pu0=tri1A*testAxis;
			pu1=tri1B*testAxis;
			pu2=tri1C*testAxis;
		}
		pv0=tri2A*testAxis;
		pv1=tri2B*testAxis;
		pv2=tri2C*testAxis;

		if(getMax(pv0,pv1,pv2)>getMin(pu0,pu1,pu2))
		{
			tpen=getMax(pv0,pv1,pv2)-getMin(pu0,pu1,pu2);
			if(tpen<pen&&tpen>0.0f)
			{
				pen=tpen;
				axis=10;
				cnAxis=testAxis;
			}
		}
		else
		{
			return;
		}
		if(getMin(pv0,pv1,pv2)<getMax(pu0,pu1,pu2))
		{
			tpen=getMax(pu0,pu1,pu2)-getMin(pv0,pv1,pv2);
			if(tpen<pen&&tpen>0.0f)
			{
				// note may need 2 invert contact normal
				pen=tpen;
				axis=11;
				cnAxis=testAxis;
			}
		}
		else
		{
			return;
		}
	}
	//set2
	testAxis = tri1cb%tri2ba;
	testAxis.Normalize();
	if(testAxis.GetMagSquared()>0.0001)
	{
		pu0=tri1A*testAxis;
		pu1=tri1B*testAxis;
		pu2=tri1C*testAxis;

		if(getMin(pu0,pu1,pu2)<0.0f)
		{
			testAxis.invert();
			pu0=tri1A*testAxis;
			pu1=tri1B*testAxis;
			pu2=tri1C*testAxis;
		}
		pv0=tri2A*testAxis;
		pv1=tri2B*testAxis;
		pv2=tri2C*testAxis;

		if(getMax(pv0,pv1,pv2)>getMin(pu0,pu1,pu2))
		{
			tpen=getMax(pv0,pv1,pv2)-getMin(pu0,pu1,pu2);
			if(tpen<pen&&tpen>0.0f)
			{
				pen=tpen;
				axis=12;
				cnAxis=testAxis;
			}
		}
		else
		{
			return;
		}
		if(getMin(pv0,pv1,pv2)<getMax(pu0,pu1,pu2))
		{
			tpen=getMax(pu0,pu1,pu2)-getMin(pv0,pv1,pv2);
			if(tpen<pen&&tpen>0.0f)
			{
				// note may need 2 invert contact normal
				pen=tpen;
				axis=13;
				cnAxis=testAxis;
			}
		}
		else
		{
			return;
		}
	}

	testAxis = tri1cb%tri2cb;
	testAxis.Normalize();
	if(testAxis.GetMagSquared()>0.0001)
	{
		pu0=tri1A*testAxis;
		pu1=tri1B*testAxis;
		pu2=tri1C*testAxis;

		if(getMin(pu0,pu1,pu2)<0.0f)
		{
			testAxis.invert();
			pu0=tri1A*testAxis;
			pu1=tri1B*testAxis;
			pu2=tri1C*testAxis;
		}
		pv0=tri2A*testAxis;
		pv1=tri2B*testAxis;
		pv2=tri2C*testAxis;

		if(getMax(pv0,pv1,pv2)>getMin(pu0,pu1,pu2))
		{
			tpen=getMax(pv0,pv1,pv2)-getMin(pu0,pu1,pu2);
			if(tpen<pen&&tpen>0.0f)
			{
				pen=tpen;
				axis=14;
				cnAxis=testAxis;
			}
		}
		else
		{
			return;
		}
		if(getMin(pv0,pv1,pv2)<getMax(pu0,pu1,pu2))
		{
			tpen=getMax(pu0,pu1,pu2)-getMin(pv0,pv1,pv2);
			if(tpen<pen&&tpen>0.0f)
			{
				// note may need 2 invert contact normal
				pen=tpen;
				axis=15;
				cnAxis=testAxis;
			}
		}
		else
		{
			return;
		}
	}

	testAxis = tri1cb%tri2ac;
	testAxis.Normalize();
	if(testAxis.GetMagSquared()>0.0001)
	{
		pu0=tri1A*testAxis;
		pu1=tri1B*testAxis;
		pu2=tri1C*testAxis;

		if(getMin(pu0,pu1,pu2)<0.0f)
		{
			testAxis.invert();
			pu0=tri1A*testAxis;
			pu1=tri1B*testAxis;
			pu2=tri1C*testAxis;
		}
		pv0=tri2A*testAxis;
		pv1=tri2B*testAxis;
		pv2=tri2C*testAxis;

		if(getMax(pv0,pv1,pv2)>getMin(pu0,pu1,pu2))
		{
			tpen=getMax(pv0,pv1,pv2)-getMin(pu0,pu1,pu2);
			if(tpen<pen&&tpen>0.0f)
			{
				pen=tpen;
				axis=16;
				cnAxis=testAxis;
			}
		}
		else
		{
			return;
		}
		if(getMin(pv0,pv1,pv2)<getMax(pu0,pu1,pu2))
		{
			tpen=getMax(pu0,pu1,pu2)-getMin(pv0,pv1,pv2);
			if(tpen<pen&&tpen>0.0f)
			{
				// note may need 2 invert contact normal
				pen=tpen;
				axis=17;
				cnAxis=testAxis;
			}
		}
		else
		{
			return;
		}
	}
	// set3
	testAxis = tri1ac%tri2ba;
	testAxis.Normalize();
	if(testAxis.GetMagSquared()>0.0001)
	{
		pu0=tri1A*testAxis;
		pu1=tri1B*testAxis;
		pu2=tri1C*testAxis;

		if(getMin(pu0,pu1,pu2)<0.0f)
		{
			testAxis.invert();
			pu0=tri1A*testAxis;
			pu1=tri1B*testAxis;
			pu2=tri1C*testAxis;
		}
		pv0=tri2A*testAxis;
		pv1=tri2B*testAxis;
		pv2=tri2C*testAxis;

		if(getMax(pv0,pv1,pv2)>getMin(pu0,pu1,pu2))
		{
			tpen=getMax(pv0,pv1,pv2)-getMin(pu0,pu1,pu2);
			if(tpen<pen&&tpen>0.0f)
			{
				pen=tpen;
				axis=18;
				cnAxis=testAxis;
			}
		}
		else
		{
			return;
		}
		if(getMin(pv0,pv1,pv2)<getMax(pu0,pu1,pu2))
		{
			tpen=getMax(pu0,pu1,pu2)-getMin(pv0,pv1,pv2);
			if(tpen<pen&&tpen>0.0f)
			{
				// note may need 2 invert contact normal
				pen=tpen;
				axis=19;
				cnAxis=testAxis;
			}
		}
		else
		{
			return;
		}
	}

	testAxis = tri1ac%tri2cb;
	testAxis.Normalize();
	if(testAxis.GetMagSquared()>0.0001)
	{
		pu0=tri1A*testAxis;
		pu1=tri1B*testAxis;
		pu2=tri1C*testAxis;

		if(getMin(pu0,pu1,pu2)<0.0f)
		{
			testAxis.invert();
			pu0=tri1A*testAxis;
			pu1=tri1B*testAxis;
			pu2=tri1C*testAxis;
		}
		pv0=tri2A*testAxis;
		pv1=tri2B*testAxis;
		pv2=tri2C*testAxis;

		if(getMax(pv0,pv1,pv2)>getMin(pu0,pu1,pu2))
		{
			tpen=getMax(pv0,pv1,pv2)-getMin(pu0,pu1,pu2);
			if(tpen<pen&&tpen>0.0f)
			{
				pen=tpen;
				axis=20;
				cnAxis=testAxis;
			}
		}
		else
		{
			return;
		}
		if(getMin(pv0,pv1,pv2)<getMax(pu0,pu1,pu2))
		{
			tpen=getMax(pu0,pu1,pu2)-getMin(pv0,pv1,pv2);
			if(tpen<pen&&tpen>0.0f)
			{
				// note may need 2 invert contact normal
				pen=tpen;
				axis=21;
				cnAxis=testAxis;
			}
		}
		else
		{
			return;
		}
	}

	testAxis = tri1ac%tri2ac;
	testAxis.Normalize();
	if(testAxis.GetMagSquared()>0.0001)
	{
		pu0=tri1A*testAxis;
		pu1=tri1B*testAxis;
		pu2=tri1C*testAxis;

		if(getMin(pu0,pu1,pu2)<0.0f)
		{
			testAxis.invert();
			pu0=tri1A*testAxis;
			pu1=tri1B*testAxis;
			pu2=tri1C*testAxis;
		}
		pv0=tri2A*testAxis;
		pv1=tri2B*testAxis;
		pv2=tri2C*testAxis;

		if(getMax(pv0,pv1,pv2)>getMin(pu0,pu1,pu2))
		{
			tpen=getMax(pv0,pv1,pv2)-getMin(pu0,pu1,pu2);
			if(tpen<pen&&tpen>0.0f)
			{
				pen=tpen;
				axis=22;
				cnAxis=testAxis;
			}
		}
		else
		{
			return;
		}
		if(getMin(pv0,pv1,pv2)<getMax(pu0,pu1,pu2))
		{
			tpen=getMax(pu0,pu1,pu2)-getMin(pv0,pv1,pv2);
			if(tpen<pen&&tpen>0.0f)
			{
				// note may need 2 invert contact normal
				pen=tpen;
				axis=23;
				cnAxis=testAxis;
			}
		}
		else
		{
			return;
		}
	}

	if(axis<6)
	{
		if(axis<3)
		{
			DragonContact *DC = new DragonContact();
			DC->m_vContactPoint=cp;
			DC->m_vContactNormal = cnAxis;
			DC->m_rPenetration=pen;
			DC->SetContactData(mesh2,mesh1,m_Restitution);
			m_vContacts.push_back(DC);
			return;// the 2 tris over lap completely on the same plane contact gen is any vert normal of either tri 0 pen
		}
		else
		{
			DragonContact *DC = new DragonContact();
			DC->m_vContactPoint=cp;
			DC->m_vContactNormal = cnAxis;
			DC->m_rPenetration=pen;
			DC->SetContactData(mesh1,mesh2,m_Restitution);
			m_vContacts.push_back(DC);
			return;// the 2 tris over lap completely on the same plane contact gen is any vert normal of either tri 0 pen
		}
	}
	else if(axis>=6&&axis<=11)
	{
		cp=TriEdgePointOfContact(tri1A,tri1B,tri2C,tri2B,tri2A);
		DragonContact *DC = new DragonContact();
		DC->m_vContactPoint=cp;
		DC->m_vContactNormal = cnAxis;
		DC->m_rPenetration=pen;
		if(axis==7||axis==9||axis==11)
		{
			DC->SetContactData(mesh2,mesh1,m_Restitution);
		}
		else
		{
			DC->SetContactData(mesh1,mesh2,m_Restitution);
		}
		m_vContacts.push_back(DC);
		return;// the 2 tris over lap completely on the same plane contact gen is any vert normal of either tri 0 pen
	}
	else if(axis>=12&&axis<=17)
	{
		cp=TriEdgePointOfContact(tri1B,tri1C,tri2C,tri2B,tri2A);
		DragonContact *DC = new DragonContact();
		DC->m_vContactPoint=cp;
		DC->m_vContactNormal = cnAxis;
		DC->m_rPenetration=pen;
		if(axis==13||axis==15||axis==17)
		{
			DC->SetContactData(mesh2,mesh1,m_Restitution);
		}
		else
		{
			DC->SetContactData(mesh1,mesh2,m_Restitution);
		}
		m_vContacts.push_back(DC);
		return;// the 2 tris over lap completely on the same plane contact gen is any vert normal of either tri 0 pen
	}
	else if(axis>=18&&axis<=23)
	{
		cp=TriEdgePointOfContact(tri1C,tri1A,tri2C,tri2B,tri2A);
		DragonContact *DC = new DragonContact();
		DC->m_vContactPoint=cp;
		DC->m_vContactNormal = cnAxis;
		DC->m_rPenetration=pen;
		if(axis==19||axis==21||axis==23)
		{
			DC->SetContactData(mesh2,mesh1,m_Restitution);
		}
		else
		{
			DC->SetContactData(mesh1,mesh2,m_Restitution);
		}
		m_vContacts.push_back(DC);
		return;// the 2 tris over lap completely on the same plane contact gen is any vert normal of either tri 0 pen
	}
}

bool DragonContactRegistry::SectorRayCollisionDetection(DragonXVector3& spherePos,real rad,DragonXVector3& rayOrigin,DragonXVector3& rayDirection)
{
	DragonXVector3 m = rayOrigin-spherePos;
	real c = (m*m)-(rad*rad);

	if(c<=0.0f)
	{
		return true;
	}

	real b = m*rayDirection;

	if(b>0.0f)
	{
		return false;
	}

	real disc = b*b-c;

	if(disc < 0.0f)
	{
		return false;
	}
	else
	{
		return true;
	}
}
// gen contact mesh ray
void DragonContactRegistry::GenerateContactsMeshRay(BaseGameEntity* obj1,BaseGameEntity* obj2)
{
	if(obj1->GetPhysicsPointer()->mI_PrimitiveType==PT_Mesh)// if obj1 is the mesh
	{
		for(auto sector : obj1->GetPhysicsPointer()->GetSectors())
		{
			if(SectorRayCollisionDetection(obj1->GetPhysicsPointer()->getPointInWorldSpace(sector->GetPosition()),sector->GetRadius(),obj2->GetPhysicsPointer()->getPosition(),obj2->GetPhysicsPointer()->GetNormal()))
			{
				for(auto tri : sector->GetDragonTris())
				{
					GenerateContactsRayTri(obj1,tri,obj2);
				}
			}
		}
	}
	else// obj2 is mesh
	{
		for(auto sector : obj2->GetPhysicsPointer()->GetSectors())
		{
			if(SectorRayCollisionDetection(obj2->GetPhysicsPointer()->getPointInWorldSpace(sector->GetPosition()),sector->GetRadius(),obj1->GetPhysicsPointer()->getPosition(),obj1->GetPhysicsPointer()->GetNormal()))
			{
				for(auto tri : sector->GetDragonTris())
				{
					GenerateContactsRayTri(obj2,tri,obj1);
				}
			}
		}
	}
}

// tri ray contact gen
void DragonContactRegistry::GenerateContactsRayTri(BaseGameEntity* mesh, DragonTriangle* tri,BaseGameEntity* ray)
{
	DragonXVector3 rayNormal = ray->GetPhysicsPointer()->GetNormal();
	DragonXVector3 a=mesh->GetPhysicsPointer()->getPointInWorldSpace(tri->GetVertA());
	DragonXVector3 b=mesh->GetPhysicsPointer()->getPointInWorldSpace(tri->GetVertB());
	DragonXVector3 c=mesh->GetPhysicsPointer()->getPointInWorldSpace(tri->GetVertC());
	DragonXVector3 m = rayNormal%ray->GetPhysicsPointer()->getPosition();
	real u = (rayNormal*(c%b))+(m*(c-b));
	if(u<0.0f)
	{
		return;
	}
	real v = (rayNormal*(a%c))+(m*(a-c));
	if(v<0.0f)
	{
		return;
	}
	real w = (rayNormal*(b%a))+(m*(b-a));
	if(w<0.0f)
	{
		return;
	}
	real denom=1.0f/(u+v+w);
	u*=denom;
	v*=denom;
	w*=denom;
	DragonXVector3 rayPointOfContact = u*a+v*b+w*c;
	DragonContact *DC = new DragonContact();
	DC->m_vContactPoint=rayPointOfContact;
	DC->m_vContactNormal = rayNormal;
	DC->m_rPenetration=(rayPointOfContact-ray->GetPhysicsPointer()->getPosition()).GetMagnitude();
	DC->SetContactData(mesh,ray,m_Restitution);
	m_RayContacts.push_back(DC);
	return;// the 2 tris over lap completely on the same plane contact gen is any vert normal of either tri 0 pen
}