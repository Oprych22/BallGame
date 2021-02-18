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


#include "Thing3D.h"
#include "World.h"
#include "Nig.h"


//**************************************************************************//
// Constructors.															//
//**************************************************************************//

Thing3D::Thing3D() 
{
}

Thing3D::Thing3D(ID3D10Device *renderingDevice) : Thing3DAbstract(renderingDevice)
{
}

Thing3D::Thing3D(ID3D10Device *renderingDevice, bool creature)  : Thing3DAbstract(renderingDevice)
{
	xSpeed = 0;
	ySpeed = 0;
	zSpeed = 0;
	groundY    = 1.0f;
	bouncyness = 0.3f;
	g=9.8f;
	numberOfBlocks = 3;
	airBorn = true;
	allowedToMove = true;
}

Thing3D::Thing3D(ID3D10Device *renderingDevice, float xe, float ze, float ye) : Thing3DAbstract(renderingDevice)
{
	xSpeed = 0;
	ySpeed = 0;
	zSpeed = 0;
	xSize = xe;
	zSize = ze;
	ySize = ye;
	groundY    = 1.0f;
	bouncyness = 0.4f;
	g=9.8f;
	airBorn = true;
	destructable = true;
	deadly = false;

}




//**************************************************************************//
// Destructor.   We should release some Dx stuff here.   It is NOT complete!//
//**************************************************************************//




Thing3D::~Thing3D()
{

	int j = 1;
}

void Thing3D::setWorld(World* newWorld)
{
	world = newWorld;
}

void Thing3D::jump()
{
	
	if (!airBorn)
	{
		y+=0.5;
		ySpeed += 6;
	}
	airBorn = true;

}

bool Thing3D::isBlocked(int xIn, int yIn, int zIn)
{
	
		
	if (world->getThingPos(xIn,yIn,zIn) )
	{
		return true;
	}
	else
	{
		return false;
	}
	
}

bool Thing3D::getPos(int xIn, int yIn, int zIn)
{
	if (x == xIn && y == yIn && z == zIn)
	{
		return true;
	}
	else
	{
		return false;
	}

}



void Thing3D::move()
{
	
	
		float adjustedXSpeed = 0;
		float adjustedZSpeed = 0;
	if (allowedToMove)
	{
	

		//BLocked X
		if ((!isBlocked((int)x+1,(int) y+1, (int)z+1) && !isBlocked((int)x+1,(int) y+1, (int)z+1)))//&& xSpeed != 0)
		{
			adjustedXSpeed = xSpeed *frameTime;
		}
		else
		{
			xSpeed = -xSpeed;
			adjustedXSpeed = xSpeed *frameTime;

		}
		//BLocked Z
		if ((!isBlocked((int)x+1,(int) y+1, (int)z+1) && !isBlocked((int)x+1,(int) y+1, (int)z+1)))//&& zSpeed != 0)
		{
			adjustedZSpeed = zSpeed *frameTime;
		}
		else
 		{
			zSpeed = -zSpeed;
			adjustedZSpeed = zSpeed *frameTime;
		}
	}

	
		killPlayer();
	

		Gravity();
		 x +=  adjustedXSpeed; 
		 z +=  adjustedZSpeed;
}


void Thing3D::destroyBlock()
{
	
	if (world->getThing((int)(x+1),(int) (y-0.5), (int)(z+1)) != NULL)
	{
		world->getThing((int)(x+1),(int) (y-0.5), (int)(z+1))->materialAmbientColour = D3DXVECTOR4(1.0f, 1.0f, 1.0f,1.0f);
		world->removeThing((int)(x+1),(int) (y-0.5), (int)(z+1));
		numberOfBlocks++;
	}
}


void Thing3D::buildBlock()
{
	
	if (world->getThing((int)(x+1),(int) (y+0.5), (int)(z+1)) == NULL)
	{
		
		if (world->addThing((int)(x+1),(int) (y+0.5), (int)(z+1)))
		{
			y += 1.8;
		}
	}
}

void Thing3D::buildBlockLeft()
{
	
	if (world->getThing((int)(x-1),(int) (y+0.5), (int)(z+1)) == NULL && numberOfBlocks > 0)
	{
		
		if (world->addThing((int)(x-1),(int) (y+0.5), (int)(z+1)))
		{
			numberOfBlocks--;
		}
	}
}

void Thing3D::buildBlockRight()
{
	
	if (world->getThing((int)(x+2.5),(int) (y+0.5), (int)(z+1)) == NULL && numberOfBlocks > 0)
	{
		
		if (world->addThing((int)(x+2.5),(int) (y+0.5), (int)(z+1)))
		{
			numberOfBlocks--;
		}
	}
}
void Thing3D::buildBlockUp()
{
	
	if (world->getThing((int)(x+1),(int) (y+0.5), (int)(z+2.5)) == NULL && numberOfBlocks > 0)
	{
		
		if (world->addThing((int)(x+1),(int) (y+0.5), (int)(z+2.5)))
		{
			numberOfBlocks--;
		}
	}
}

void Thing3D::buildBlockDown()
{
	
	if (world->getThing((int)(x+1),(int) (y+0.5), (int)(z-1)) == NULL && numberOfBlocks > 0)
	{
		
		if (world->addThing((int)(x+1),(int) (y+0.5), (int)(z-1)))
		{
			numberOfBlocks--;
		}
	}
}

void Thing3D::killPlayer()
{
	if (allowedToMove)
	{
		if (world->getThing((int)(x+1),(int) (y-0.5), (int)(z+1)) != NULL)
		{
			if (world->getThing((int)(x+1),(int) (y-0.5), (int)(z+1))->deadly)
			{
				world->getThing((int)(x+1),(int) (y-0.5), (int)(z+1))->materialAmbientColour = D3DXVECTOR4(1.0f, 1.0f, 1.0f,1.0f);
				allowedToMove = false;
				ySpeed=0.1;
			}
			if (world->getThing((int)(x+1),(int) (y-0.5), (int)(z+1))->type == 3)
			{
				world->levelFinished = true;
				allowedToMove = false;
				
			}
		
		}
	}
}

void Thing3D::Gravity()
{
	if (allowedToMove)
	{
	FLOAT deltaX = xSpeed*frameTime;  
	FLOAT deltaY = ySpeed*frameTime; 
	FLOAT deltaZ = zSpeed*frameTime;


	ySpeed -= g*(frameTime);	  

	x += deltaX; y += deltaY;	z += deltaZ;   
	
		if ((!isBlocked((int)x+1,(int) y+0.5, (int)z+1) && !isBlocked((int)x+1,(int) y+0.5, (int)z+1)))
		{
		
		
		}
		else
		{
			ySpeed = -ySpeed*bouncyness;	
			//y = groundY+0.5;		

			if (ySpeed < 0.2 && ySpeed > -0.2)
			{
				airBorn = false;
			}
			else 
			{
				airBorn = true;
			}

			if ( ySpeed != 0)
			{
				FLOAT scaleY = (sy - ((ySpeed*0.99)/80));
				setScale(0.05, scaleY, 0.05);
			}

			

			//if (!airBorn)
			//{
			//	ySpeed = 0;
			//}
		}

	
		if (ySpeed < 0.3)
		{
			setScale(0.05, 0.05, 0.05);
		}
	}

	else
	{
		if (world->levelFinished == false)
		{
			y-=1.0*frameTime;
			if (y < 0)
			{
				moveTo(6, 16, 25);
				allowedToMove = true;
			}
		}
		
	}
	
	
	
	
	
}

//**************************************************************************//
// Create method.  It uses the VERTEX format that was defined in			//
// AbstractThing3D.	  However we could certainly devine our own custom		//
// format here.																//
//**************************************************************************//

void Thing3D::create() 
{
	HRESULT hr;		//Used by the V() macro.  

   

	UINT numElements = sizeof( abstractThing3DVertexLayout ) / sizeof( abstractThing3DVertexLayout[0] );

	
    // Create the input layout It uses the VERTEX format that was defined in	//
    // AbstractThing3D.															//
    D3D10_PASS_DESC PassDesc;
    pTechnique->GetPassByIndex( 0 )->GetDesc( &PassDesc );
    hr = pd3dDevice->CreateInputLayout( abstractThing3DVertexLayout, numElements, PassDesc.pIAInputSignature,
                                     PassDesc.IAInputSignatureSize, (ID3D10InputLayout**) &pVertexLayout);

	V(hr);

    // Set the input layout
    pd3dDevice->IASetInputLayout( pVertexLayout );

    // Load the mesh
}




//**************************************************************************//
// Load the mesh from an sdkmesh file.  This must be called AFTER			//
// createEffect().															//
//**************************************************************************//
	
void Thing3D::createMesh(LPCTSTR SDKMeshFileName)
{
	HRESULT hr;		//Used by the V() macro.  
	
	// Define the input layout

	UINT numElements = sizeof( abstractThing3DVertexLayout ) / sizeof( abstractThing3DVertexLayout[0] );

	
    // Create the input layout
    D3D10_PASS_DESC PassDesc;
    pTechnique->GetPassByIndex( 0 )->GetDesc( &PassDesc );
    hr = pd3dDevice->CreateInputLayout( abstractThing3DVertexLayout, numElements, PassDesc.pIAInputSignature,
                                     PassDesc.IAInputSignatureSize, (ID3D10InputLayout**) &pVertexLayout);

	V(hr);

    // Set the input layout
    pd3dDevice->IASetInputLayout( pVertexLayout );

    // Load the mesh
	V( mesh.Create( pd3dDevice, SDKMeshFileName, true ) );

}





//**************************************************************************//
// Implementation of an abstract method in the parent class.  Render ourself//
// for whatever we are in this particular implementation, which is a mesh.	//
//**************************************************************************//

void Thing3D::renderForMyImplementation() 											
{

	if (pd3dDevice == NULL)
	{
		ShowMessage(L"Thing3D: Gimme a rendering device!");
		exit(1);	//Brutal exit!  No cleaning up.
	}


	setUpWorldMatrix();
	matWorldViewProjection = matWorld * matView * matProjection;


	//**********************************************************************//
	// Pass parameters to the shader file.  We pass the lighting parameters //
	// every time we do a render(), which is probably silly, but the light  //
	// may have moved; we never know...										//
	//																		//
	//**********************************************************************//

	pMatWorldViewProjInShader->SetMatrix(          (float *) &matWorldViewProjection);
	pMatWorldInShader->SetMatrix(                  (float *) &matWorld);
	pMaterialDiffuseColourInShader->SetFloatVector((float *) &materialDiffuseColour);
	pMaterialAmbientColourInShader->SetFloatVector((float *) &materialAmbientColour);

	pd3dDevice->IASetInputLayout( pVertexLayout );


	UINT Strides[1];
    UINT Offsets[1];
    ID3D10Buffer* pVB[1];
    pVB[0] = mesh.GetVB10( 0, 0 );
    Strides[0] = ( UINT )mesh.GetVertexStride( 0, 0 );
    Offsets[0] = 0;
    pd3dDevice->IASetVertexBuffers( 0, 1, pVB, Strides, Offsets );
    pd3dDevice->IASetIndexBuffer( mesh.GetIB10( 0 ), mesh.GetIBFormat10( 0 ), 0 );

    D3D10_TECHNIQUE_DESC techDesc;
    pTechnique->GetDesc( &techDesc );
    SDKMESH_SUBSET* pSubset = NULL;
    ID3D10ShaderResourceView* pDiffuseRV = NULL;
    D3D10_PRIMITIVE_TOPOLOGY PrimType;


	
	for( UINT p = 0; p < techDesc.Passes; ++p )
    {
        for( UINT subset = 0; subset < mesh.GetNumSubsets( 0 ); ++subset )
        {
            pSubset = mesh.GetSubset( 0, subset );

            PrimType = mesh.GetPrimitiveType10( ( SDKMESH_PRIMITIVE_TYPE )pSubset->PrimitiveType );
            pd3dDevice->IASetPrimitiveTopology( PrimType );

            pDiffuseRV = mesh.GetMaterial( pSubset->MaterialID )->pDiffuseRV10;
            ptxDiffuseInShader->SetResource( pDiffuseRV );

            pTechnique->GetPassByIndex( p )->Apply( 0 );
            pd3dDevice->DrawIndexed( ( UINT )pSubset->IndexCount, 0, ( UINT )pSubset->VertexStart );
        }
  
	}
}









//**************************************************************************//
// Do something, up to you what.	All this does is move the object forward//
// and slow it down some, to simulate slow the object a wee bit of friction.//
//**************************************************************************//

void Thing3D::doBehaviour() 
{
	//moveForward();
	move();
	xSpeed *= 0.95;
	zSpeed *= 0.95;
}
