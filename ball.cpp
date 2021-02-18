
#include "ball.h"


//**************************************************************************//
// Constructors.															//
//**************************************************************************//

Ball::Ball(ID3D10Device *renderingDevice, bool creature) : Thing3D(renderingDevice, creature)
{
	
}



void Ball::setUpWorldMatrix()
{
    //Rotation about x, y and z matrices, and a translation matrix.

    D3DXMatrixRotationX( &matRotateX, rx);
    D3DXMatrixRotationY( &matRotateY, ry);
    D3DXMatrixRotationZ( &matRotateZ, rz);
	
	//axisRotation=0;
	

	//currentPosition.x = xSpeed*100;
	//currentPosition.y = ySpeed*100;
	if (xSpeed > 0)
	{
		currentPosition.z = -1;
		axisRotation += xSpeed /50;
	}
	else if (xSpeed < 0)
	{
		currentPosition.z = 1;
		axisRotation -= xSpeed /50;
	}
	currentPosition.y = 0;
	if (zSpeed > 0)
	{
		currentPosition.x = 1;
		if (xSpeed != 0)
		{
			axisRotation += zSpeed /70;
		}
		else
		{
			axisRotation += zSpeed /50;
		}
	}
	else if (zSpeed < 0)
	{
		currentPosition.x = -1;
		if (xSpeed != 0)
		{
			axisRotation -= zSpeed /70;
		}
		else
		{
			axisRotation -= zSpeed /50;
		}
	}

	

	

		D3DXVec3Normalize(&currentPosition, &currentPosition);
		D3DXMatrixRotationAxis(&matRotateAxis, &currentPosition, axisRotation);

	

    //D3DXMatrixMultiply( &matAllRotations, &matRotateX, &matRotateY);
    //D3DXMatrixMultiply( &matAllRotations, &matRotateZ, &matAllRotations);

    // I forgot.   This is C++, where you can "overload" the assignment //
    // and arithmetic operators.   ...So you can just multiply and add  //
    // matrices like they were any other variable - I have commented    //
    // out the D3DXMatrixMultiply stuff 'cos it can be replaced with    //
    // what is below.                                                   //

    matAllRotations = /*matRotateX * matRotateY * matRotateZ **/ matRotateAxis;

    //Now scale it before we translate it.

    D3DXMATRIX matScale;
    D3DXMatrixScaling(&matScale, sx, sy, sz);
    D3DXMatrixTranslation( &matTranslate, x, y, z);

    // If we want to both rotate and translate, we must multiply both matrices
    // together.   This multiplies matRotateY and matTranslate to produce
    // matTransformed.   The order of multiplication is important.   Try
    // reversing it and see if you can understand what you get.

	if (xSpeed == 0 && zSpeed == 0)
	{
		matWorld =  matScale * matTranslate;

	}
	else
	{
		matWorld = matRotateAxis * matScale * matTranslate;
	}
      
}