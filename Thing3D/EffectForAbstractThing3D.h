//**************************************************************************//
// Class to implement an effect that goes in conjunction with the class		//
// Thing3DAbstract.     It is basically just a container for an effect that //
// many instances of a child if a Thing3DAbstract may share.				//
// It is not yet finished.													//
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




#ifndef EffectForAbstractThing3D_H     //These are termed "guards", can you see what they do?
#define EffectForAbstractThing3D_H



class EffectForAbstractThing3D
{

	//**********************************************************************//
	// Public instance variables.  OO purists will want to make these		//
	// private and write the setThis() and getThat() methods.				//
	//**********************************************************************//

	public:
		ID3D10Effect	*pEffect;	//The effect we hold.

	  //**********************************************************************//
	  // This is where encapsulation falls down some.  The shader file has its//
	  // lighting attributes.  This doesn't really make sense surely light is //
	  // a globl thing, not an attribute of an object?  It makes partial sense//
	  // here, as I am assuming that in a simple world. all objects will share//
	  // the same shader.													  //
	  //**********************************************************************//

	  D3DXVECTOR4		lightDirection;				//w value unused.
	  D3DXVECTOR4		lightDiffuseColour;			//w value unused.
	  D3DXVECTOR4		lightAmbientColour;


	  //**********************************************************************//
	  // There are all variables we will use to access variables within the	  //
	  // shader (.fx) file.													  //
	  //**********************************************************************//

	  ID3D10EffectVectorVariable		  *pLightDiffuseColourInShader;
	  ID3D10EffectVectorVariable		  *pLightAmbientColourInShader;
	  ID3D10EffectVectorVariable	      *pLightDirectionInShader;



	//**********************************************************************//
	// Public methods.														//
	//**********************************************************************//

   public:
		EffectForAbstractThing3D();					//Constructors.
		EffectForAbstractThing3D(ID3D10Device *renderingDevice, 
			                     LPCTSTR fileName); //Create from a file name.

		~EffectForAbstractThing3D();				//Destructor.

		//******************************************************************//
		// Create effect from an "fx" file.									//
		//******************************************************************//

		void create(ID3D10Device *renderingDevice, LPCTSTR fileName);

};    //Must be a ";" here in C++ - weird, eh?   Nigel









#endif	//End of guard.