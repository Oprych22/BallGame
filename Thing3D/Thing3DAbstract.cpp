//**************************************************************************//
// Class to implement a Thing3DAbstract.     It is not yet finished.        //
//																			//
// This class is an ABSTRACT class; you cannot make it with a new().  You	//
// must sub-class it and omplement the abatract methods in the sub class.	//
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
#include "DXUT.h"
#include "DXUTgui.h"
#include "DXUTmisc.h"
#include "DXUTSettingsDlg.h"
#include "SDKmisc.h"
#include "SDKmesh.h"
#include <string.h>
#include <wchar.h>






//**************************************************************************//
// This lot all return void, but it would be better to have many of them    //
// return an HRESULT to indicate success/failure.                           //
//**************************************************************************//


//**************************************************************************//
// Constructors.															//
//**************************************************************************//

Thing3DAbstract::Thing3DAbstract() 
{
	init();  
}


Thing3DAbstract::Thing3DAbstract(ID3D10Device *renderingDevice)
{
	init();
	pd3dDevice = renderingDevice;
	pd3dDevice->AddRef();
}



//**************************************************************************//
// Used by constructors.													//
//**************************************************************************//

void Thing3DAbstract::init()
{
   x  = y  = z  = forwardSpeed = 0.0f;
   rx = ry = rz         = 0.0f;
   sx = sy = sz         = 1.0f;
   initVecDir.x    = 1;     initVecDir.y = 0;     initVecDir.z = 0;
   frameTime	   = 0.001;
   pd3dDevice	   = NULL;

   materialDiffuseColour = D3DXVECTOR4(1, 1, 1, 1);
   materialAmbientColour = D3DXVECTOR4(0.3f, 0.3f, 0.3f, 0.3f);
   currentPosition.x = x;
    currentPosition.y = y;
	 currentPosition.z = z;
	 axisRotation = 0;
}



//**************************************************************************//
// Destructor.   We should release some Dx stuff here.						//
//**************************************************************************//

Thing3DAbstract::~Thing3DAbstract()
   {
	   SAFE_RELEASE(pVertexLayout);
	   SAFE_RELEASE(pd3dDevice);
   }



//**************************************************************************//
// Set the effect and plug several parameters into it.	Ah, a setter, you	//
// may say, but it does rahter more than just assign a variable.			//
//**************************************************************************//

void Thing3DAbstract::setEffect(EffectForAbstractThing3D *effect)
{

	pThing3DEffect = effect;


	//**********************************************************************//
	// Obtain the the variables in the shader file that will correspond to	//
	// parameters, technique, matrices and such.							//
	//**********************************************************************//

	pTechnique          = pThing3DEffect->pEffect->GetTechniqueByName( "RenderScene" );
    ptxDiffuseInShader  = pThing3DEffect->pEffect->GetVariableByName( "g_MeshTexture" )->AsShaderResource();
    
	pMatWorldViewProjInShader = pThing3DEffect->pEffect->GetVariableByName( "g_mWorldViewProjection" )->AsMatrix();
    pMatWorldInShader         = pThing3DEffect->pEffect->GetVariableByName( "g_mWorld" )->AsMatrix();
    pfTimeInShader            = pThing3DEffect->pEffect->GetVariableByName( "g_fTime" )->AsScalar();
	
	pMaterialDiffuseColourInShader = pThing3DEffect->pEffect->GetVariableByName ("g_MaterialDiffuseColor")->AsVector();
	pMaterialAmbientColourInShader = pThing3DEffect->pEffect->GetVariableByName ("g_MaterialAmbientColor")->AsVector();
}






//**************************************************************************//
// Move to an absolute position.                                            //
//**************************************************************************//

void Thing3DAbstract::moveTo(float newX, float newY, float newZ)
   {
   x = newX; y = newY; z = newZ;
   }


//**************************************************************************//
// Move relative to the current position.                                   //
//**************************************************************************//

void Thing3DAbstract::moveBy(float byX,  float byY,  float byZ)
   {
   x += byX; y += byY; z += byZ;
   }


//**************************************************************************//
// Rotate to an absolute angle.                                             //
//**************************************************************************//

void Thing3DAbstract::rotateTo(float newRX,  float newRY,  float newRZ)
   {
   rx = newRX; ry = newRY; rz = newRZ;
   }


//**************************************************************************//
// Rotate relative to current orientation.                                  //
//**************************************************************************//

void Thing3DAbstract::rotateBy(float byRX,  float byRY,  float byRZ)
   {
   rx += byRX; ry += byRY; rz += byRZ;

   float twoPi = D3DX_PI*2;

   if (rx > twoPi) rx -= twoPi;         //Just to stop the rotation angle
   if (ry > twoPi) ry -= twoPi;         //going above/below one circle.
   if (rz > twoPi) rz -= twoPi;

   if (rx < 0) rx += twoPi;
   if (ry < 0) ry += twoPi;
   if (rz < 0) rz += twoPi;
   }



//*************************************************************************//
// Set the scale of the object.                                            //
//*************************************************************************//

void Thing3DAbstract::setScale(float newsx,   float newsy,   float newsz)
   {
   sx = newsx;   sy = newsy;   sz = newsz;   
   }



//**************************************************************************//
// Set the object's initial direction - used by the moveForward method.     //
//**************************************************************************//

void Thing3DAbstract::setInitialDirection(float initX, float initY, float initZ)
   {
   initVecDir.x = initX;   initVecDir.y = initY;   initVecDir.z = initZ;
   }


//**************************************************************************//
// Move the object forward in the direction it is pointing, at the speed it //
// is going.                                                                //
//                                                                          //
// If anybody can come up with a better solution, see Nigel!                //
//                                                                          //
// We start by creating a vector pointing down the x axis (default is       //
// 1, 0, 0), then multiply this vector by the combined rotation matrices    //
// (x, y, z) rotations.                                                     //
//                                                                          //
// We normalise the resultane vector (i.e. give it a magnitude of 1),       //
// extract the x, y, z components, and multiply them by the object's speed. //
//                                                                          //
// I haven't attampted to make this code independent of the frame rate yet! //
//                                                                          //
// Nigel.                                                                   //
//**************************************************************************//

void Thing3DAbstract::moveForward()
    {
    

	//*******************************************************************//
	// Help!   I need to re-create the rotation matrices here; I have no //
    // idea why.   The whole point of making the matrices members of this//
    // class was so that I need calculate them only once.   Remove the   //
    // lines to the ------------- and this should still work, but doesn't//
    // - Nigel.                                                          //
	//*******************************************************************//
	/*
	if (forwardSpeed > 0)
	{
		rx = forwardSpeed/5;
	}
	*/
    D3DXMatrixRotationX( &matRotateX, rx);
    D3DXMatrixRotationY( &matRotateY, ry);
    D3DXMatrixRotationZ( &matRotateZ, rz);

	

    matAllRotations = matRotateX * matRotateY * matRotateZ;

    //-------------------------------------------------------------------//

    D3DXVECTOR3 newVecDir;

    D3DXVec3TransformCoord(&newVecDir, &initVecDir, &matAllRotations);
    D3DXVec3Normalize( &currentVecDir, &newVecDir );

    float adjustedSpeed = forwardSpeed*frameTime;
    
    x +=  currentVecDir.x*adjustedSpeed;  y +=  currentVecDir.y*adjustedSpeed;
    z +=  currentVecDir.z*adjustedSpeed;
    }



//**************************************************************************//
// Draw yourself - and do the matrix stuff as well...                       //
//**************************************************************************//

void Thing3DAbstract::render()
   {
	   setUpWorldMatrix();              //Do the movement stuff.
	   renderForMyImplementation();
   }









//**************************************************************************//
// Set up the world matrix.													//
//**************************************************************************//

void Thing3DAbstract::setUpWorldMatrix()
{
    //Rotation about x, y and z matrices, and a translation matrix.

    D3DXMatrixRotationX( &matRotateX, rx);
    D3DXMatrixRotationY( &matRotateY, ry);
    D3DXMatrixRotationZ( &matRotateZ, rz);

    //D3DXMatrixMultiply( &matAllRotations, &matRotateX, &matRotateY);
    //D3DXMatrixMultiply( &matAllRotations, &matRotateZ, &matAllRotations);

    // I forgot.   This is C++, where you can "overload" the assignment //
    // and arithmetic operators.   ...So you can just multiply and add  //
    // matrices like they were any other variable - I have commented    //
    // out the D3DXMatrixMultiply stuff 'cos it can be replaced with    //
    // what is below.                                                   //

    matAllRotations = matRotateX * matRotateY * matRotateZ ;

    //Now scale it before we translate it.

    D3DXMATRIX matScale;
    D3DXMatrixScaling(&matScale, sx, sy, sz);
    D3DXMatrixTranslation( &matTranslate, x, y, z);

    // If we want to both rotate and translate, we must multiply both matrices
    // together.   This multiplies matRotateY and matTranslate to produce
    // matTransformed.   The order of multiplication is important.   Try
    // reversing it and see if you can understand what you get.

     matWorld = matAllRotations * matScale * matTranslate; 
}




//**************************************************************************//
// This is an extremely simple method to test whether two things intersect. //
// And it only works with objects that are of its own kind (or children of  //
// its own kind).															//
//																			//
// So go write something better.											//
//**************************************************************************//

bool Thing3DAbstract::intersectsWith(Thing3DAbstract *thing)
{
	float dx = x - thing->x;
	float dy = y - thing->y;
	float dz = z - thing->z;

	dy = 0;
	float dist = (float) sqrt(dx*dx + dy*dy + dz*dz);

	float averageScale = (sx + sy + sz) / 3.0f;

	if (averageScale > dist) return true;


	return false;
}







