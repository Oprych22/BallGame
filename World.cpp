#include "World.h"
#include "Thing3D.h"
#include "Nig.h"
using namespace std;

//**************************************************************************//
// Constructors.															//
//**************************************************************************//



World::World(ID3D10Device* renderingDevice )
{
	pd3dDevice = renderingDevice;
	pd3dDevice->AddRef();
	levelFinished = false;
	allThings = NULL;

}

World::World(ID3D10Device* renderingDevice, string fname )
{
	pd3dDevice = renderingDevice;
	pd3dDevice->AddRef();
	fileName = fname;
	levelFinished = false;
	startingLocationX = 6;
	startingLocationY = 16;
	startingLocationZ = 25;
	loading = true;
	allThings = NULL;

}

World::~World()
{
	
	SAFE_RELEASE(pd3dDevice);

	if (allThings == NULL)
	{
		return;
	}
	
	
	for (int i = 0; i < 10; i++) 
	{
		for (int j = 0; j < 30; j++)
		{
			for (int k = 0; k < 30; k++)
			{
				Thing3D * newThing = allThings[i][j][k];
				SAFE_DELETE(newThing);
			}
			delete [] allThings[i][j];
		}
		delete [] allThings[i];
	}
	delete [] allThings;

	
}

void World::setEffect(EffectForAbstractThing3D * specialEffect)
{

	g_p_Effect = specialEffect;
}



bool World::getThingPos(int xIn, int yIn, int zIn)
{
if (allThings[(yIn/2)][zIn/2][xIn/2] != NULL)
	{
		return true;
	}
	else
	{
		return false;
	}

}

void World::removeThing(int newx, int newy, int newz)
{
	if (getThingPos(newx, newy, newz))
	{
		if (allThings[newy/2][newz/2][newx/2]->destructable)
		{
			allThings[newy/2][newz/2][newx/2] = NULL;
		}
	}

}

bool World::addThing(int newx, int newy, int newz)
{
	if (getThingPos(newx, newy, newz) == false)
	{
		if (allThings[(int)(newy-1)/2][(int)newz/2][(int)newx/2] != NULL)
		{
			thing = new Thing3D(pd3dDevice, 1, 1, 1);

			thing->setEffect(g_p_Effect);
			thing->createMesh(L"Media\\Cube\\Cube.sdkmesh");
			thing->setScale(0.1, 0.1, 0.1);
		
			if (newx % 2 != 0)
			{
				newx -= 1;
			}
			if (newz % 2 != 0)
			{
				newz -= 1;
			}
		
			thing->moveTo((int)(newx), (int)(newy), (int)(newz));

			allThings[(int)newy/2][(int)newz/2][(int)newx/2] = thing;
			return true;
		}
	}
	return false;

}

void World::createWorld()
{
	allThings = new Thing3D***[10];
	for (int i = 0; i < 10; i++) 
	{
		allThings[i] = new Thing3D**[30];
		for (int j = 0; j < 30; j++)
		{
			allThings[i][j] = new Thing3D*[30];
		}
	}

	int tempArray[10][30][30];
	;
	ifstream infile;
	
	infile.open(fileName);
	
	unsigned char ch;
	unsigned int y = 0;
	unsigned int x  = 0;
	unsigned int z = 0;

	while(infile.good())
	{
		ch = infile.get();

		if (ch == '\n')
		{
			z += 1;
			x=0;
			if (z >= 30 )
			{
				z= 0;
				y+=1;
			}
			if (y >= 10 )
			{
				infile.close();
				break;

			}
		}
		else if (ch == 'X' || ch == 'S' || ch == 'R' || ch == 'O')
		{
			
			if (x == 30 || z == 30)
			{
				cout << "";
			}
			if (ch == 'X' || ch == 'R')
			{
				thing = new Thing3D(pd3dDevice, 1, 1, 1);

				thing->setEffect(g_p_Effect);
				if (y == 0 )
				{
						
					thing->createMesh(L"Media\\Lava\\Lava.sdkmesh");
					thing->destructable = false;
					thing->deadly = true;

				}
				else
				{
					thing->createMesh(L"Media\\Cube\\Cube.sdkmesh");
				}
					

				thing->setScale(0.1, 0.1, 0.1);
				thing->moveTo((float)(x*2), (float)(y*2), (float)(z*2));
				if (ch == 'X')
				{
					thing->type = 1;
				}
				else if (ch == 'R' || ch == 'S')
				{
					thing->type = 2;
				}




				allThings[y][z][x] = thing;


			}
			else if (ch == 'O')
			{
				allThings[y][z][x] = NULL;
			}
			else if (ch == 'S')
			{
				thing = new Thing3D(pd3dDevice, 1, 1, 1);

				thing->setEffect(g_p_Effect);
				thing->createMesh(L"Media\\Cube\\Cube.sdkmesh");
					
				thing->setScale(0.1, 0.1, 0.1);
				thing->moveTo((float)(x*2), (float)(y*2), (float)(z*2));
				thing->type = 3;
				thing->materialDiffuseColour = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 1.0f);
				thing->materialAmbientColour = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 1.0f);
				allThings[y][z][x] = thing;
				
			}
			
			
			x+=1;
		}
		else
		{
			//allThings[y][z][x] = NULL;
		}	
	}
}



		

void World::renderAllCubes(D3DXMATRIX matView, D3DXMATRIX matProjection)
{
	for (int x = 0; x < 30; x++)
	{
		for (int z = 0; z < 30; z++)
		{
			for (int y = 9; y >= 0; y--)
			{
				if
				((((y < 9)  && allThings[y+1][z][x] != NULL) && 
					((z < 29 && z > 0) && (allThings[y][z+1][x] != NULL && allThings[y][z-1][x] != NULL))) && 
					((x < 29 && x > 0) && (allThings[y][z][x+1] != NULL && allThings[y][z][x-1] != NULL)))
				{
					break;
				}

				if (z == 0 && y > 4)
				{
					continue;
				}

				if (allThings[y][z][x] != NULL)
				{
					
					thing = allThings[y][z][x];

					if (thing->type == 2 || thing->type == 3)
					{
						thing->rotateBy(0, 0.2, 0);
					}
					thing->matView       = matView;
					thing->matProjection = matProjection;


					thing->render();
				}

			}
		}
	}
}
Thing3D* World::getThing(int newx, int newy, int newz)
{

	if (getThingPos(newx, newy, newz) != NULL)
	{
			
		return allThings[newy/2][newz/2][newx/2];

	}
	else
	{
		return NULL;
	}
	

	
}
