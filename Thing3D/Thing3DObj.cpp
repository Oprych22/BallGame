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


#include "Thing3DObj.h"
#include "Nig.h"


//**************************************************************************//
// Constructors.															//
//**************************************************************************//

Thing3DObj::Thing3DObj() 
{
}

Thing3DObj::Thing3DObj(ID3D10Device *renderingDevice)
{
	pd3dDevice = renderingDevice;
}



//**************************************************************************//
// Destructor.   We should release some Dx stuff here.   It is NOT complete!//
//**************************************************************************//

Thing3DObj::~Thing3DObj()
{
}						


//**************************************************************************//
// Create method.  It uses the VERTEX format that was defined in			//
// AbstractThing3D.	  However we could certainly devine our own custom		//
// format here.																//
//**************************************************************************//

void Thing3DObj::create() 
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
	
void Thing3DObj::createMesh(LPCTSTR OBJMeshFileName)
{
	HRESULT hr;		//Used by the V() macro.  
	UINT numElements = sizeof( abstractThing3DVertexLayout ) / sizeof( abstractThing3DVertexLayout[0] );
	D3D10_PASS_DESC PassDesc;
    pTechnique->GetPassByIndex( 0 )->GetDesc( &PassDesc );
    V( pd3dDevice->CreateInputLayout( abstractThing3DVertexLayout, numElements, PassDesc.pIAInputSignature,
                                             PassDesc.IAInputSignatureSize, &pVertexLayout ) );

    pd3dDevice->IASetInputLayout( pVertexLayout );

    // Load the mesh
    V( objMesh.Create( pd3dDevice, OBJMeshFileName ) );




    // Store the correct technique for each material
	for ( UINT i = 0; i < objMesh.GetNumMaterials(); ++i )
    {
        Material* pMaterial = objMesh.GetMaterial( i );

        const char* strTechnique = "";

        if( pMaterial->pTextureRV10 && pMaterial->bSpecular )
            strTechnique = "TexturedSpecular";
        else if( pMaterial->pTextureRV10 && !pMaterial->bSpecular )
            strTechnique = "TexturedNoSpecular";
        else if( !pMaterial->pTextureRV10 && pMaterial->bSpecular )
            strTechnique = "Specular";
        else if( !pMaterial->pTextureRV10 && !pMaterial->bSpecular )
            strTechnique = "NoSpecular";


		// For the minute, force the Technique to be "render"
		pMaterial->pTechnique = pThing3DEffect->pEffect->GetTechniqueByName( "RenderScene" );
    }

 
}





//**************************************************************************//
// Implementation of an abstract method in the parent class.  Render ourself//
// for whatever we are in this particular implementation, which is a mesh.	//
//**************************************************************************//

void Thing3DObj::renderForMyImplementation() 											
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



	//
    // Set the Vertex Layout
    //
 	pd3dDevice->IASetInputLayout( pVertexLayout );

	D3D10_TECHNIQUE_DESC techDesc;
    pTechnique->GetDesc( &techDesc );
    

   
    //
    // Render the mesh
    //
	int numSubsets = objMesh.GetNumSubsets();

    for ( UINT iSubset = 0; iSubset < numSubsets; ++iSubset )
    {
        renderSubset( iSubset );
    }
   

}


//**************************************************************************//
// Render a subset of the mesh.												//
//**************************************************************************//

void Thing3DObj::renderSubset( UINT iSubset )
{
    HRESULT hr;
    
    Material* pMaterial = objMesh.GetSubsetMaterial( iSubset );

    // g_pAmbient->SetFloatVector( pMaterial->vAmbient ) );
    // g_pDiffuse->SetFloatVector( pMaterial->vDiffuse ) );
    // g_pSpecular->SetFloatVector( pMaterial->vSpecular ) );
    // g_pOpacity->SetFloat( pMaterial->fAlpha ) );
    // g_pSpecularPower->SetInt( pMaterial->nShininess ) );

    if ( !IsErrorResource( pMaterial->pTextureRV10 ) )
        ptxDiffuseInShader->SetResource( pMaterial->pTextureRV10 );

    D3D10_TECHNIQUE_DESC techDesc;
    pMaterial->pTechnique->GetDesc( &techDesc );

    for ( UINT p = 0; p < techDesc.Passes; ++p )
    {
        pMaterial->pTechnique->GetPassByIndex(p)->Apply(0);
        objMesh.GetMesh()->DrawSubset(iSubset);
    }
}








//**************************************************************************//
// Do something, up to you what.	All this does is move the object forward//
// and slow it down some, to simulate slow the object a wee bit of friction.//
//**************************************************************************//

void Thing3DObj::doBehaviour() 
{
	moveForward();
	forwardSpeed *= 0.99;
}
