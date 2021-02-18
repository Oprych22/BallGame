

#include "DXUT.h"
#include <vector>
#include "Thing3D.h"
#include "World.h"

#include "ball.h"



#ifndef levelLoaderH     
#define levelLoaderH



class levelLoader 
{
public:

	std::vector <World *> levels;	
	int currentLevel;
	ID3D10Device * pd3dDevice;
	EffectForAbstractThing3D* g_p_Effect;
	//int test[];

public:
	levelLoader(ID3D10Device* renderingDevice, EffectForAbstractThing3D* g_p_Effect);
	~levelLoader();
	void loadLevel(Ball* thing);
	void renderLevel(D3DXMATRIX matView, D3DXMATRIX matProjection);
	void nextLevel();
	
};


#endif