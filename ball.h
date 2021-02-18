
#include "Thing3D.h"



#ifndef BallH     //These are termed "guards", can you see what they do?
#define BallH	 


//**************************************************************************//
// Many of these member variables are public and have to be set by writing  //
// to the variable.   OO programmers would have us write many more          //
// setThisandThat(...) methods.                                             //
//**************************************************************************//


class Ball : public Thing3D
{

	//**********************************************************************//
	// Public instance variables.											//
	//**********************************************************************//

	public:

		Ball(ID3D10Device *renderingDevice, bool creature) ;
		void setUpWorldMatrix();

};
#endif