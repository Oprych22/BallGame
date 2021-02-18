#include "World.h"
#include "ball.h"
#include "Nig.h"
#include "levelLoader.h"
#include "Thing3D.h"
#include <iostream>
#include <fstream>
using namespace std;


//**************************************************************************//
// Constructors.															//
//**************************************************************************//



levelLoader::levelLoader(ID3D10Device* renderingDevice, EffectForAbstractThing3D* g_p_Effect)
{
	pd3dDevice = renderingDevice;
	currentLevel = 0;
	World* world1 = new World(pd3dDevice, "Level\\Level1.txt");
	world1->setEffect(g_p_Effect);

	levels.push_back(world1);

	World* world2 = new World(pd3dDevice, "Level\\Level2.txt");
	world2->setEffect(g_p_Effect);

	levels.push_back(world2);

	World* world3 = new World(pd3dDevice, "Level\\Level3.txt");
	world3->setEffect(g_p_Effect);

	levels.push_back(world3);
	

}

void levelLoader::loadLevel(Ball* thing)
{
	levels.at(currentLevel)->createWorld();
	levels.at(currentLevel)->loading = false;
	thing->setWorld(levels.at(currentLevel));
	
	thing->moveTo(levels.at(currentLevel)->startingLocationX, 
				  levels.at(currentLevel)->startingLocationY, 
				  levels.at(currentLevel)->startingLocationZ);
	thing->allowedToMove==true;

}

void levelLoader::renderLevel(D3DXMATRIX matView, D3DXMATRIX matProjection)
{
	levels.at(currentLevel)->renderAllCubes(matView, matProjection);

}

void levelLoader::nextLevel()
{
	currentLevel++;
}

levelLoader::~levelLoader()
{
	int levelSize = levels.size()-1;

	for (int i = levelSize; i >= 0 ; i--)
	{
		
		World* neworld = levels.at(i);
		levels.erase(levels.begin()+i);
		SAFE_DELETE(neworld);
	}

}



