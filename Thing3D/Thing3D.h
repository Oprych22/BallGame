//**************************************************************************//
// Class to implement a Thing3d.     It is a child of the abstract class	//
// "Thing3DAbstract.h.														//
//**************************************************************************//

//**************************************************************************//
// This code is copyright of Dr Nigel Barlow, lecturer in computing,		//
// University of Plymouth, UK.  email: nigel@soc.plymouth.ac.uk.			//
//																			//
// You may use, modify and distribute this (rather cack-handed in places)	//
// code subject to the following conditions:								//
//																			//
//	1:	You may not use it, or sell it, or use it in any adapted form for	//
//		financial gain, without my written premission.						//
//																			//
//	2:	You must not remove the copyright messages.							//
//																			//
//	3:	You should correct at least 10% of the typig abd spekking errirs.   //
//**************************************************************************//


#include "Thing3DAbstract.h"
#include "SDKmesh.h"



#ifndef Thing3DH     //These are termed "guards", can you see what they do?
#define Thing3DH	 


//**************************************************************************//
// Many of these member variables are public and have to be set by writing  //
// to the variable.   OO programmers would have us write many more          //
// setThisandThat(...) methods.                                             //
//**************************************************************************//
class World;

class Thing3D : public Thing3DAbstract
{

	//**********************************************************************//
	// Public instance variables.											//
	//**********************************************************************//

	public:
		CDXUTSDKMesh mesh;
		float frameTime;
		float xSpeed, ySpeed, zSpeed, g, groundY, bouncyness;
		float xSize, ySize, zSize;
		bool airBorn, bounce, destructable, deadly, allowedToMove;
		int type;
		int numberOfBlocks;
		World* world;

	//**********************************************************************//
	// Public methods.														//
	//**********************************************************************//

	public:
      Thing3D();									//Constructors.
	  Thing3D(ID3D10Device *renderingDevice);
	  Thing3D(ID3D10Device *renderingDevice, bool creature);
	  Thing3D(ID3D10Device *renderingDevice, float xe, float ze, float ye);
      ~Thing3D();									//Destructor.

	  

	//**********************************************************************//
	// Implementation of abstract methods in parent class.					//
    //**********************************************************************//

	public:
      virtual void create();
	  virtual void renderForMyImplementation();		   //Draw yourself 
	  												   //for whatever you are
	  virtual void doBehaviour();
	  void Gravity();
	  void jump();
	  void move();
	  bool isBlocked(int x, int y, int z);
	  bool getPos(int xIn, int yIn, int zIn);
	  void setWorld(World* newWorld);
	  void destroyBlock();
	  void buildBlock();
	  void buildBlockUp();
	  void buildBlockDown();
	  void buildBlockLeft();
	  void buildBlockRight();

	  void deleteThis();
	  void killPlayer();
	  

      //**********************************************************************//
	  // Load the mesh from an sdkmesh file.  This must be called AFTER	      //
	  // createEffect().											 		  //
	  //**********************************************************************//
	 
	  virtual void createMesh(LPCTSTR SDKMeshFileName);//Load graphic from file.

	  



};    //Must be a ";" here in C++ - weird, eh?   Nigel


#endif	//End of guard.

