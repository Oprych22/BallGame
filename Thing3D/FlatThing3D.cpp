//**************************************************************************//
// Implements a flat thing in 3d space, which can be used as a background	//
// - or anything flat :-)													//
//																			//
//	It is a child of the abstract class										//
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

#include "FlatThing3D.h"
#include "Nig.h"



//**************************************************************************//
// Structure to define a Vertex.  Thid is linked in with the shader we use, //
// and this won't work unless you have a shader with a posotion, normal		//
// vector and textureUV coordinates.										//
//**************************************************************************//

struct SimpleFlatThingVertex
{
    D3DXVECTOR3 pos;
	D3DXVECTOR3 normal;
    D3DXVECTOR2 texUV;
};



//**************************************************************************//
// Constructors.															//
//**************************************************************************//

FlatThing3D::FlatThing3D() 
{
	init();
	textureAssigned = false;
}



FlatThing3D::FlatThing3D(ID3D10Device *renderingDevice)
{
	init();
	pd3dDevice      = renderingDevice;
	textureAssigned = false;
}



//**************************************************************************//
// Used by constructors.													//
//**************************************************************************//

void FlatThing3D::init()
{
   x  = y  = z  = forwardSpeed = 0.0f;
   ry = rz              = 0.0f;
   rx					= D3DXToRadian(270.0f);		//Orientation for background.
   sx = sy = sz         = 1.0f;
   initVecDir.x    = 1;     initVecDir.y = 0;     initVecDir.z = 0;
   frameTime	   = 0.001;
   pd3dDevice	   = NULL;

   materialDiffuseColour = D3DXVECTOR4(1, 1, 1, 1);
   pRenderTargetView     = NULL;
}




//**************************************************************************//
// Destructor.   We should release some Dx stuff here.   It is NOT complete!
//**************************************************************************//

FlatThing3D::~FlatThing3D()
{

}						


//**************************************************************************//
// Create; currently empty.													//
//**************************************************************************//

void FlatThing3D::create()
{

}

//**************************************************************************//
// Load the mesh from an sdkmesh file.  This must be called AFTER			//
// createEffect().															//
//**************************************************************************//
	
void FlatThing3D::createTexture(LPCTSTR textureFileName)
{
	HRESULT hr;		//Used by the V() macro.  


    UINT numElements = sizeof( abstractThing3DVertexLayout ) / sizeof( abstractThing3DVertexLayout[0] );
	
    // Create the input layout
    D3D10_PASS_DESC PassDesc;
	hr = pTechnique->GetPassByIndex( 0 )->GetDesc( &PassDesc );
	V(hr);

    hr = pd3dDevice->CreateInputLayout( abstractThing3DVertexLayout, numElements, PassDesc.pIAInputSignature,
                                     PassDesc.IAInputSignatureSize, (ID3D10InputLayout**) &pVertexLayout);
	V(hr);

    // Set the input layout
    pd3dDevice->IASetInputLayout( pVertexLayout );

    // Create VERTEX buffer
    SimpleFlatThingVertex vertices[] =
    {
        { D3DXVECTOR3( -1.0f, 0.0f, -1.0f ), D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR2( 0.0f, 0.0f ) },
        { D3DXVECTOR3( 1.0f, 0.0f, -1.0f ),  D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR2( 1.0f, 0.0f ) },
        { D3DXVECTOR3( 1.0f, 0.0f, 1.0f ),   D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR2( 1.0f, 1.0f ) },
        { D3DXVECTOR3( -1.0f, 0.0f, 1.0f ),  D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR2( 0.0f, 1.0f ) },
    };

    D3D10_BUFFER_DESC bd;
    bd.Usage = D3D10_USAGE_DEFAULT;
    bd.ByteWidth = sizeof( SimpleFlatThingVertex ) * 4;
    bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;
    bd.MiscFlags = 0;
    D3D10_SUBRESOURCE_DATA InitData;
    InitData.pSysMem = vertices;
    hr = pd3dDevice->CreateBuffer( &bd, &InitData, &pVERTEXBuffer );
    V( hr ) 
     
    
    //D3D10_BUFFER_DESC bd;
    bd.Usage = D3D10_USAGE_DEFAULT;
    bd.ByteWidth = sizeof( SimpleFlatThingVertex ) * 4;
    bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;
    bd.MiscFlags = 0;
    //D3D10_SUBRESOURCE_DATA InitData;
    InitData.pSysMem = vertices;
	
    // Create index buffer
    DWORD indices[] =
    {
        3,1,0,
        2,1,3    
	};

	
	InitData.pSysMem = indices;
    hr = pd3dDevice->CreateBuffer( &bd, &InitData, &pIndexBuffer );

    // Load the Texture
   	hr = D3DX10CreateShaderResourceViewFromFile( pd3dDevice, textureFileName, NULL, NULL, 
		&ptxtResourceViewInShader, NULL );
    V( hr );	
}





//**************************************************************************//
// Implementation of an abstract method in the parent class.  Render ourself//
// for whatever we are in this particular implementation, which is a mesh.	//
//**************************************************************************//

void FlatThing3D::renderForMyImplementation() 											
{
	if (pd3dDevice == NULL)
	{
		ShowMessage(L"Flat Thing3D: Gimme a rendering device!");
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




	//**********************************************************************//
	// If we have already drawn one of these things, we can speed up		//
	// performance (considerably!!) by not repeating this next block.  But	//
	// this only works when drawing multople objects of the same type!		//
	//**********************************************************************//

	if (!textureAssigned) 
	{
		// Set Vertex buffer
		pd3dDevice->IASetInputLayout( pVertexLayout );

		UINT stride = sizeof( SimpleFlatThingVertex );
		UINT offset = 0;
		pd3dDevice->IASetVertexBuffers( 0, 1, &pVERTEXBuffer, &stride, &offset );
		pd3dDevice->IASetIndexBuffer( pIndexBuffer, DXGI_FORMAT_R32_UINT, 0 );

		// Set primitive topology
		pd3dDevice->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );


		//Set the texture.

		ptxDiffuseInShader->SetResource( ptxtResourceViewInShader );
		}

	D3D10_TECHNIQUE_DESC techDesc;
    pTechnique->GetDesc( &techDesc );
    for( UINT p = 0; p < techDesc.Passes; ++p )
    {
        pTechnique->GetPassByIndex( p )->Apply( 0 );
        pd3dDevice->DrawIndexed(6, 0, 0 );
    }


}

	


	
//**************************************************************************//
// Do something, up to you what.											//
//**************************************************************************//

void FlatThing3D::doBehaviour()
{

}










