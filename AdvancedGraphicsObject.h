#pragma once

#include "BaseGraphicsEntity.h"

class AdvancedGraphicsObject : public BaseGraphicsEntity
{
private:

public:
	AdvancedGraphicsObject(BaseGameEntity* Owner);
	~AdvancedGraphicsObject();
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