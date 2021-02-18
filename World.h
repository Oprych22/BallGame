

#include "Tile.h"
#include "DXUT.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
using namespace std;



#ifndef WorldH    
#define WorldH

class Thing3D;

class World 
{
public:


	int testArray[3][30][30];
	Thing3D **** allThings;
	Thing3D * thing;
	ID3D10Device * pd3dDevice;
	EffectForAbstractThing3D* g_p_Effect;
	string fileName;
	bool levelFinished, loading;
	int startingLocationX;
	int startingLocationY;
	int startingLocationZ;

public:
	World();
	World(ID3D10Device* device);
	World(ID3D10Device* device, string fname);
	~World();
	void renderAllRoad(D3DXMATRIX matView, D3DXMATRIX matProjection);
	void newRoad();
	void returnRoad();
	void addCubes();
	bool getCubePos(int x, int y, int z);
	bool getThingPos(int x, int y, int z);
	Thing3D * getThing(int x, int y, int z);
	void renderAllCubes(D3DXMATRIX matView, D3DXMATRIX matProjection);
	void setEffect(EffectForAbstractThing3D *specialEffect);
	void removeThing(int newx, int newy, int newz);
	bool addThing(int newx, int newy, int newz);
	void createWorld();
};


#endif