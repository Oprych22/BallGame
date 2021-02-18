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

#include "Thing3DAbstract.h"



#ifndef FlatThing3DH     //These are termed "guards", can you see what they do?
#define FlatThing3DH	 


class FlatThing3D : public Thing3DAbstract
{

	//**********************************************************************//
	// Public instance variables.											//
	//**********************************************************************//
	
	public:
		ID3D10RenderTargetView	  *pRenderTargetView;
		ID3D10Buffer			  *pVERTEXBuffer;
		ID3D10Buffer			  *pIndexBuffer;
		ID3D10ShaderResourceView  *ptxtResourceViewInShader;

		bool					   textureAssigned;

	//**********************************************************************//
	// Implementation of abstract methods in parent class.					//
    //**********************************************************************//

	public:
      virtual void create();
	  virtual void renderForMyImplementation();		   //Draw yourself 
													   //for whatever you are
	  virtual void doBehaviour();


	//**********************************************************************//
	// Constructors and destructors.										//
	//**********************************************************************//

	public:
      FlatThing3D();									//Constructors.
	  FlatThing3D(ID3D10Device *renderingDevice);
      ~FlatThing3D();									//Destructor.


	//**********************************************************************//
	// Public methods.														//
	//**********************************************************************//

	public:
	  virtual void createTexture(LPCTSTR textureFileName);
													   //for whatever you are

    //**********************************************************************//
	// Protected and private methods.										//
	//**********************************************************************//

	protected:
	  void init(); // Used by constructors.	



};    //Must be a ";" here in C++ - weird, eh?   Nigel


#endif	//End of guard.

