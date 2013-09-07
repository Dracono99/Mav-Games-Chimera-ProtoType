#ifndef GFX_STATS_H
#define GFX_STATS_H

#include <d3d9.h>
#include <d3dx9.h>
#include <dxerr9.h>

class GfxStats
{
public:
	GfxStats();
	~GfxStats();

	void onLostDevice();
	void onResetDevice();
	float getFPS()
	{
		return mFPS;
	}

	void addVertices(DWORD n);
	void subVertices(DWORD n);
	void addTriangles(DWORD n);
	void subTriangles(DWORD n);
	void addObject();

	void setTriCount(DWORD n);
	void setVertexCount(DWORD n);

	void update(float dt);
	void display();

private:
	// Prevent copying
	GfxStats(const GfxStats& rhs);
	GfxStats& operator=(const GfxStats& rhs);

private:
	ID3DXFont* mFont;
	float mFPS;
	float mMilliSecPerFrame;
	DWORD mNumTris;
	DWORD mNumVertices;
	DWORD mNumObjects;
};
#endif // GFX_STATS_H