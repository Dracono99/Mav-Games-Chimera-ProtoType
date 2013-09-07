#pragma once

#include "d3dUtility.h"

class BaseGameEntity;

class BaseGraphicsEntity
{
private:

public:
	BaseGraphicsEntity(BaseGameEntity* Owner)
	{
		mP_Owner=Owner;
		m_AdvancedGfxObj=false;
		m_Damaged=false;
		m_UnDamagedDiffuseTex=NULL;
		m_UnDamagedNormTex=NULL;
		m_UnDamagedSpecTex=NULL;
		m_DamagedDiffuseTex=NULL;
		m_DamagedNormalTex=NULL;
		m_DamagedSpecTex=NULL;
		m_RenderedDiffuseTex=NULL;
		m_RenderedNormTex=NULL;
		m_RenderedSpecTex=NULL;
		m_VertexDispMagTex=NULL;
		m_VertexNormQuatTex=NULL;
		m_RenderedDiffuseSurface=NULL;
		m_RenderedNormSurface=NULL;
		m_RenderedSpecSurface=NULL;
		m_VertexDispMagSurface=NULL;
		m_VertexNormQuatSurface=NULL;
	}
	virtual~BaseGraphicsEntity(){}
	ID3DXMesh* mMesh;
	IDirect3DTexture9* mDiffuse;
	IDirect3DTexture9* mNormal;
	IDirect3DTexture9* mSpec;
	BaseGameEntity* mP_Owner;
	bool m_AdvancedGfxObj;
	bool m_Damaged;
	IDirect3DTexture9* m_UnDamagedDiffuseTex;
	IDirect3DTexture9* m_UnDamagedNormTex;
	IDirect3DTexture9* m_UnDamagedSpecTex;
	IDirect3DTexture9* m_DamagedDiffuseTex;
	IDirect3DTexture9* m_DamagedNormalTex;
	IDirect3DTexture9* m_DamagedSpecTex;
	IDirect3DTexture9* m_RenderedDiffuseTex;
	IDirect3DTexture9* m_RenderedNormTex;
	IDirect3DTexture9* m_RenderedSpecTex;
	IDirect3DTexture9* m_VertexDispMagTex;
	IDirect3DTexture9* m_VertexNormQuatTex;
	LPDIRECT3DSURFACE9 m_RenderedDiffuseSurface;
	LPDIRECT3DSURFACE9 m_RenderedNormSurface;
	LPDIRECT3DSURFACE9 m_RenderedSpecSurface;
	LPDIRECT3DSURFACE9 m_VertexDispMagSurface;
	LPDIRECT3DSURFACE9 m_VertexNormQuatSurface;
};
