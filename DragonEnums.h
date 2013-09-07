#pragma once

enum collisionobjectType
{
	COT_World,
	COT_Game,
};

enum primitiveType
{
	PT_Plane,
	PT_Sphere,
	PT_OBB,
	PT_KDOP,
	PT_Ray,
	PT_Mesh,
};

enum CollisionType
{
	CT_SphereSphere,
	CT_SpherePlane,
	CT_SphereOBB,
	CT_ObbPlane,
	CT_ObbObb,
	CT_SphereKdop,
	CT_OBBKDop,
	CT_KDopPlane,
	CT_KDopKDop,
	CT_RaySphere,
	CT_RayOBB,
	CT_MeshSphere,
	CT_MeshOBB,
	CT_MeshMesh,
	CT_MeshRay,
};

enum ContactType
{
	CRT_Natural,
	CRT_Ray,
};

enum HydraOctreeParts
{
	Top_Front_Left,		//0
	Top_Front_Right,	//1
	Top_Back_Left,		//2
	Top_Back_Right,		//3
	Bottom_Front_Left,	//4
	Bottom_Front_Right,	//5
	Bottom_Back_Left,	//6
	Bottom_Back_Right,	//7
};