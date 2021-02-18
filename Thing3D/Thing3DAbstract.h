//**************************************************************************//
// Class to implement a Thing3d.     It is not yet finished.                //
//                                                                          //
// All a but messier than Java - you declare the members and prototypes for //
// the methods in the header file, but the implementation of the methods is //
// given in the "cpp" file.                                                 //
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

#include "Nig.h"
#include "DXUT.h"
#include "DXUTmisc.h"
#include "SDKmisc.h"
#include "EffectForAbstractThing3D.h"


#ifndef Thing3DHAbstract   //Guards, as usual.
#define Thing3DHAbstract


//**************************************************************************//
// Define a simple input layout which is shared between all subclasses of	//
// this class.																//
//**************************************************************************//

     const D3D10_INPUT_ELEMENT_DESC abstractThing3DVertexLayout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D10_INPUT_PER_VERTEX_DATA, 0 },
    };




//**************************************************************************//
// Many of these member variables are public and have to be set by writing  //
// to the variable.   OO programmers would have us write many more          //
// setThisandThat(...) methods.                                             //
//**************************************************************************//

class Thing3DAbstract
   {



	//**********************************************************************//
	// Public instance variables.  OO purists will want to make these		//
	// private and write the setThis() and getThat() methods.				//
	//**********************************************************************//
   
   public:					//Not quite like Java, but the same meaning.
      float x, y, z;        //Position.
      float forwardSpeed;          //Yes - it goes!
      float rx, ry, rz;     //Rotation about x, y, z axes.
      float sx, sy, sz;     //Scales in x, y, z directions.
      float axisRotation;
      float frameTime;						//You must specify this, I use it
											//to calculate the elapsed time 
											//between frames and such

      


 
	   //**********************************************************************//
	   // Now we hit DirectX components.  First general D3DX stuff, which is   //
	   // (up to now) independent of the DirectX version.					   //
	   //**********************************************************************//
       
      D3DXVECTOR3 initVecDir;		 //Set once only.   Initial direction.
      D3DXVECTOR3 currentVecDir;	 //Used for moveForward and culling in
									 //lated demos.
	  
	  D3DXVECTOR3 currentPosition;
	  
	  
	  
	  //***************************************************************//	  
	  //Rotation about x, y and z matrices, and a translation matrix.  //
      //I have made these members of this class so that we need only   //
      //compute them once a frame, but if you look at the "moveForward"//
      //method, you will see that the matrices are re-calculated.   I  //
      //have no idea why...      Nigel.                                //
	  //***************************************************************//

	  D3DXMATRIX     matAllRotations;   //Used for moveForward.
      D3DXMATRIX     matRotateX;
      D3DXMATRIX     matRotateY;
      D3DXMATRIX     matRotateZ;
	  D3DXMATRIX    matRotateAxis;
      D3DXMATRIX     matTranslate;
      
	  D3DXMATRIX     matWorld;			//We need to pass the world view and
	  D3DXMATRIX     matView;			//projection matrices to the snader.
      D3DXMATRIX     matProjection;
	  D3DXMATRIX	 matWorldViewProjection;


	  //**********************************************************************//
	  //DirectX10 specific stuff.											  //
	  //**********************************************************************//
	  
	  ID3D10Device		  *pd3dDevice;	// Our rendering device is an attrubute of the 
										//object.  I'm not sure this is the best way
										//to handle it in DX10.


	  EffectForAbstractThing3D			  *pThing3DEffect; 
	  ID3D10InputLayout                   *pVertexLayout;
	  ID3D10EffectTechnique               *pTechnique;


	  //**********************************************************************//
	  // Material properties; diffues creflection only here.				  //
  	  //**********************************************************************//

	  D3DXVECTOR4		materialDiffuseColour, materialAmbientColour;		//w value unused.



	  //**********************************************************************//
	  // There are all variables we will use to access variables within the	  //
	  // shader (.fx) file.													  //
	  //**********************************************************************//
	  
	  ID3D10EffectMatrixVariable		  *pMatWorldViewProjInShader;
	  ID3D10EffectMatrixVariable          *pMatWorldInShader;
      ID3D10EffectScalarVariable		  *pfTimeInShader;
	  ID3D10EffectShaderResourceVariable  *ptxDiffuseInShader;
	  ID3D10EffectVectorVariable	      *pMaterialDiffuseColourInShader;
		ID3D10EffectVectorVariable        *pMaterialAmbientColourInShader  ;




	//**********************************************************************//
	// Public methods.														//
	//**********************************************************************//

   public:
      Thing3DAbstract();								 //Constructors.
	  Thing3DAbstract(ID3D10Device *renderingDevice);

      ~Thing3DAbstract();    //Destructor.

      //*********************************************************************//
      //This lot all return void, but it would be better to have them return //
      //an HRESULT to indicate success/failure.   Or even throw an exception.//
	  //                                                                     //
      // There should be more of these to set the speed, material etc, but   //
      // as yet, this is all there is.                                       //
      //*********************************************************************//


      //*********************************************************************//
	  // ABSTRACT METHODS - you must implelemt them in a subclass.			 //
	  //																	 //
	  //*********************************************************************//
      // Various versions of "initGeometry".   The one that takes the name of//
	  // a mesh is the most useful.											 //
      // here I handle both :-)                Nigel.                        //
      //*********************************************************************//

      virtual void create()						= 0;
	  virtual void renderForMyImplementation()  = 0;	//Draw yourself 
														//for whatever you are


      //*********************************************************************//
      // This one is for you to fill in the bevaviour of the object.         //
      //*********************************************************************//
      virtual void doBehaviour() = 0;

      

	  //*********************************************************************//
	  // Set the effect and plug several parameters into it.	Ah, a setter,// 
	  // you may say, but it does rahter more than just assign a variable.	 //
	  //*********************************************************************//
	  virtual void setEffect(EffectForAbstractThing3D *effect);


     
      //********************************************************************//
      // Set the object's initial direction - used by the moveForward       //
      // method.                                                            //
      //********************************************************************//
      virtual void setInitialDirection(float initX, float initY, float initZ);



      //********************************************************************//
      // Move to an absolute position.                                      //
      //********************************************************************//
   	  virtual void moveTo(float newX, float newY, float newZ);


      //********************************************************************//
      // Move relative to the current position.                             //
      //********************************************************************//
   	  virtual void moveBy(float byX,  float byY,  float byZ);

      //*********************************************************************//
      // Move the object forward in the direction it is pointing, at the     //
      // speed it is going.  That is the difficult one!                      //
      //*********************************************************************//
   	  virtual void moveForward();


      //********************************************************************//
      // Rotate to an absolute angle.                                       //
      //********************************************************************//
   	  virtual void rotateTo(float newRX,  float newRY,  float newRZ);



      //********************************************************************//
      // Rotate relative to current orientation.                            //
      //********************************************************************//
   	  virtual void rotateBy(float byRX,   float byRY,   float byRZ);


      //********************************************************************//
      // Set the scale of the object.                                       //
      //********************************************************************//
   	  virtual void setScale(float newsx,   float newsy,   float newsz);


      //********************************************************************//
      // Draw yourself - and do the matrix stuff as well...                 //
      //********************************************************************//
      virtual void render();  //The virtual methods are replaced if a child //
                              //object has a method of the same name.       //
                              //Ask Nigel.                                  //


     
      //********************************************************************//
      // Set up the world matrix.											//
      //********************************************************************//   
	  virtual void setUpWorldMatrix();


	  //*********************************************************************//
	  // This is an extremely simple method to test whether two things		 //
	  // intersect.  And it only works with objects that are of its own kind //
	  // (or children of its own kind).										 //
	  //																	 //
	  // So go write something better.										 //
	  //*********************************************************************//
	  virtual bool intersectsWith(Thing3DAbstract *thing);



    //**********************************************************************//
	// Private and protected methods.										//
	//**********************************************************************//

	protected:
	  void init(); // Used by constructors.	


   };    //Must be a ";" here in C++ - weird, eh?   Nigel


   #endif

