//**************************************************************************//
// The DirectX9 Thing3D now ported to DirextX10.  This module is based on	//
// Microsoft’s “Simple Sample”, but I have done quite a lot of poggeling.   //
// Apart from using my own Thing3D.											//
//																			//
// This (poggled) code is copyright of Dr Nigel Barlow, lecturer in			//
// computing, University of Plymouth, UK.  email: nigel@soc.plymouth.ac.uk.	//
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
//																			//
// Poggeling ©Nigel Barlow nigel@soc.plymouth.ac.uk.						//
// "Simple Sample" ©Microsoft Corporation. All rights reserved.				//
//**************************************************************************//


#include "Nig.h"
#include "DXUT.h"
#include "DXUTgui.h"
#include "DXUTmisc.h"
#include "DXUTCamera.h"
#include "DXUTSettingsDlg.h"
#include "SDKmisc.h"
#include "SDKmesh.h"
#include "resource.h"
#include "XInput.h"
#include "windows.h"
#include "Thing3D.h"
#include "World.h"
#include "levelLoader.h"
#include "ball.h"
#include "soundEffects.h"
#include <vector>
#include <stdio.h>
#include <iostream>



#pragma comment(lib, "XInput.lib")
//#define DEBUG_VS   // Uncomment this line to debug D3D9 vertex shaders 
//#define DEBUG_PS   // Uncomment this line to debug D3D9 pixel shaders 




//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------
CModelViewerCamera          g_Camera;               // A model viewing camera
CDXUTDialogResourceManager  g_DialogResourceManager; // manager for shared resources of dialogs
CD3DSettingsDlg             g_SettingsDlg;          // Device settings dialog
CDXUTTextHelper*            g_pTxtHelper = NULL;
CDXUTDialog                 g_HUD;                  // dialog for standard controls
CDXUTDialog                 g_SampleUI;             // dialog for sample specific controls

// Direct3D 9 resources
extern ID3DXFont*           g_pFont9;
extern ID3DXSprite*         g_pSprite9;


// Direct3D 10 resources
ID3DX10Font*                g_pFont10 = NULL;
ID3DX10Sprite*              g_pSprite10 = NULL;
ID3D10InputLayout*          g_pVertexLayout = NULL;
ID3D10EffectMatrixVariable* g_pmWorldViewProj = NULL;
ID3D10EffectMatrixVariable* g_pmWorld = NULL;
ID3D10EffectScalarVariable* g_pfTime = NULL;



//**************************************************************************//
// Nigel added global variables.											//
//**************************************************************************//

EffectForAbstractThing3D		*g_p_Effect;
Ball							*thing;
Thing3D							*box, *box2;
levelLoader                     *levels;
SoundManager                   newSoundManager;
soundEffects                    sound1(&newSoundManager); 
soundEffects					sound2(&newSoundManager); 
soundEffects					sound3(&newSoundManager);

DWORD sound1State = 0;

//**************************************************************************//
// A horrible syntax to create a list (vector) of pointers to tiles, with   //
// an initial size of 0.													//
//																			//
// As it is a list of Thing3DAbstract, then we can put any child of			//
// Thing3DAbstract into it.													//
//**************************************************************************//
	

bool		g_b_LeftArrowDown      = false;	//Status of keyboard.  Thess are set
bool		g_b_RightArrowDown     = false;	//in the callback KeyboardProc(), and 
bool		g_b_UpArrowDown	       = false; //are used in onFrameMove().
bool		g_b_DownArrowDown	   = false;
bool		g_b_SpaceDown	   = false;
bool		g_b_BDown	   = false;
bool		g_b_NDown	   = false;
bool stopVibrating = false;
double timeElapsed =0;

bool g_b_thirdPersonCamera = false;
bool g_b_WorldCamera = false;
bool g_b_NonMoveCamera = false;

#define MAX_CONTROLLERS 4  // XInput handles up to 4 controllers 
#define INPUT_DEADZONE  ( 0.24f * FLOAT(0x7FFF) )  // Default to 24% of the +/- 32767 range.   This is a reasonable default value but can be altered if needed.

struct CONTROLER_STATE
{
    XINPUT_STATE state;
    bool connected;
};

CONTROLER_STATE g_Controllers[MAX_CONTROLLERS];
WCHAR g_szMessage[4][1024] = {0};
HWND    g_hWnd;
bool    g_bDeadZoneOn = true;

//--------------------------------------------------------------------------------------
// UI control IDs
//--------------------------------------------------------------------------------------
#define IDC_TOGGLEFULLSCREEN    1
#define IDC_TOGGLEREF           2
#define IDC_CHANGEDEVICE        3
#define IDC_TOGGLEWARP          4



//--------------------------------------------------------------------------------------
// Forward declarations 
//--------------------------------------------------------------------------------------
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing,
                          void* pUserContext );
void CALLBACK OnKeyboard( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext );
void CALLBACK OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext );
void CALLBACK OnFrameMove( double fTime, float fElapsedTime, void* pUserContext );
bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, void* pUserContext );

extern bool CALLBACK IsD3D9DeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat,
                                             bool bWindowed, void* pUserContext );
extern HRESULT CALLBACK OnD3D9CreateDevice( IDirect3DDevice9* pd3dDevice,
                                            const D3DSURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext );
extern HRESULT CALLBACK OnD3D9ResetDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc,
                                           void* pUserContext );
extern void CALLBACK OnD3D9FrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime,
                                        void* pUserContext );
extern void CALLBACK OnD3D9LostDevice( void* pUserContext );
extern void CALLBACK OnD3D9DestroyDevice( void* pUserContext );

bool CALLBACK IsD3D10DeviceAcceptable( UINT Adapter, UINT Output, D3D10_DRIVER_TYPE DeviceType,
                                       DXGI_FORMAT BackBufferFormat, bool bWindowed, void* pUserContext );
HRESULT CALLBACK OnD3D10CreateDevice( ID3D10Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc,
                                      void* pUserContext );
HRESULT CALLBACK OnD3D10ResizedSwapChain( ID3D10Device* pd3dDevice, IDXGISwapChain* pSwapChain,
                                          const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext );
void CALLBACK OnD3D10FrameRender( ID3D10Device* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext );
void CALLBACK OnD3D10ReleasingSwapChain( void* pUserContext );
void CALLBACK OnD3D10DestroyDevice( void* pUserContext );

void InitApp();
void RenderText();
void newRoad(ID3D10Device* pd3dDevice);
void showHelp();
void setStaticCameraPosition();
void UpdateControllerState(float fElapsedTime);



//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
    // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

    // DXUT will create and use the best device (either D3D9 or D3D10) 
    // that is available on the system depending on which D3D callbacks are set below

    // Set DXUT callbacks
    DXUTSetCallbackMsgProc( MsgProc );
    DXUTSetCallbackKeyboard( OnKeyboard );
    DXUTSetCallbackFrameMove( OnFrameMove );
    DXUTSetCallbackDeviceChanging( ModifyDeviceSettings );

    DXUTSetCallbackD3D9DeviceAcceptable( IsD3D9DeviceAcceptable );
    DXUTSetCallbackD3D9DeviceCreated( OnD3D9CreateDevice );
    DXUTSetCallbackD3D9DeviceReset( OnD3D9ResetDevice );
    DXUTSetCallbackD3D9DeviceLost( OnD3D9LostDevice );
    DXUTSetCallbackD3D9DeviceDestroyed( OnD3D9DestroyDevice );
    DXUTSetCallbackD3D9FrameRender( OnD3D9FrameRender );

    DXUTSetCallbackD3D10DeviceAcceptable( IsD3D10DeviceAcceptable );
    DXUTSetCallbackD3D10DeviceCreated( OnD3D10CreateDevice );
    DXUTSetCallbackD3D10SwapChainResized( OnD3D10ResizedSwapChain );
    DXUTSetCallbackD3D10SwapChainReleasing( OnD3D10ReleasingSwapChain );
    DXUTSetCallbackD3D10DeviceDestroyed( OnD3D10DestroyDevice );
    DXUTSetCallbackD3D10FrameRender( OnD3D10FrameRender );

    InitApp();
    DXUTInit( true, true, NULL ); // Parse the command line, show msgboxes on error, no extra command line params
    DXUTSetCursorSettings( true, true );
    DXUTCreateWindow( L"The Maze!" );
    DXUTCreateDevice( true, 640, 480 );
    DXUTMainLoop(); // Enter into the DXUT render loop

    return DXUTGetExitCode();
}


//--------------------------------------------------------------------------------------
// Initialize the app 
//--------------------------------------------------------------------------------------
void InitApp()
{
    g_SettingsDlg.Init( &g_DialogResourceManager );
    g_HUD.Init( &g_DialogResourceManager );
    g_SampleUI.Init( &g_DialogResourceManager );

    g_HUD.SetCallback( OnGUIEvent ); int iY = 10;
    g_HUD.AddButton( IDC_TOGGLEFULLSCREEN, L"Toggle full screen", 35, iY, 125, 22 );
    g_HUD.AddButton( IDC_CHANGEDEVICE, L"Change device (F2)", 35, iY += 24, 125, 22, VK_F2 );
    g_HUD.AddButton( IDC_TOGGLEREF, L"Toggle REF (F3)", 35, iY += 24, 125, 22, VK_F3 );
    g_HUD.AddButton( IDC_TOGGLEWARP, L"Toggle WARP (F4)", 35, iY += 24, 125, 22, VK_F4 );

    g_SampleUI.SetCallback( OnGUIEvent ); iY = 10;
}

void UpdateControllerState(float fElapsedTime)
{
    DWORD dwResult;
    for( DWORD i = 0; i < MAX_CONTROLLERS; i++ )
    {
        // Simply get the state of the controller from XInput.
        dwResult = XInputGetState( i, &g_Controllers[i].state );

        if( dwResult == ERROR_SUCCESS )
            g_Controllers[0].connected = true;
        else
            g_Controllers[i].connected = false;
    }


	//**********************************************************************//
	// We only use controller[0].	Look in the state.GamePad variable to   //
	// see what is availavle.												//
	//**********************************************************************//

	if (g_Controllers[0].connected)
	{
		XINPUT_VIBRATION Vibration;
		ZeroMemory(&Vibration, sizeof(XINPUT_VIBRATION));


		float newXSpeed =  g_Controllers[0].state.Gamepad.sThumbLX;	//Left Joypad 
		float newZSpeed = g_Controllers[0].state.Gamepad.sThumbLY; //Left Joypad 

		//float yCameraSpeed =  g_Controllers[0].state.Gamepad.sThumbRX;	//Left Joypad 

		//**********************************************************************//
		// Joypads have a dead zone.  To prevent noise, cero the reading if the //
		// answer is within the dead zone.										//
		//**********************************************************************//

		if ((newZSpeed <= INPUT_DEADZONE) && (newZSpeed >= -INPUT_DEADZONE )) newZSpeed = 0;
		if ((newXSpeed <= INPUT_DEADZONE) && (newXSpeed >= -INPUT_DEADZONE )) newXSpeed = 0;

		//if ((yCameraSpeed < INPUT_DEADZONE) && (yCameraSpeed > -INPUT_DEADZONE )) yCameraSpeed = 0;

		//**********************************************************************//
		// Scale the result.  Full scale is + / - 32767.  Multiply by 90  for a //
		// max ov 90 degrees and convert to radians.							//
		//**********************************************************************//
		thing->xSpeed += (((newXSpeed/ 32767.0)/70) / fElapsedTime);
		thing->zSpeed += (((newZSpeed/ 32767.0)/70) / fElapsedTime) ;

		

		if (thing->allowedToMove == false && stopVibrating == false)
		{
			Vibration.wLeftMotorSpeed = 65535;
			Vibration.wLeftMotorSpeed = 65535;

			
		}
		else
		{
			Vibration.wLeftMotorSpeed = 0;
			Vibration.wLeftMotorSpeed = 0;

			
		}
		XInputSetState(0, &Vibration);
		if(g_Controllers[0].state.Gamepad.wButtons)
		{
		}

		if(g_Controllers[0].state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP)
		{
			g_b_NonMoveCamera  = false;     
				g_b_thirdPersonCamera = true;
				g_b_WorldCamera = false;

		}
		else if(g_Controllers[0].state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN)
		{
			g_b_NonMoveCamera  = true;     
				g_b_thirdPersonCamera = false;
				g_b_WorldCamera = false;
				
				

		}
		else if (g_Controllers[0].state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT)
		{
			g_b_NonMoveCamera  = false;     
				g_b_thirdPersonCamera = false;
				g_b_WorldCamera = true;
				setStaticCameraPosition(); 
		}
		
		if(g_Controllers[0].state.Gamepad.bLeftTrigger)
		{
			thing->jump();
			

		}
		if(g_Controllers[0].state.Gamepad.bRightTrigger)
		{
			thing->destroyBlock();
			

		}


		switch (g_Controllers[0].state.Gamepad.wButtons)
		{

			case (XINPUT_GAMEPAD_A):
				sound1.play();
				thing->buildBlockDown();
			break;

			case (XINPUT_GAMEPAD_X):
				sound1.play();
				thing->buildBlockLeft();
			break;

			case (XINPUT_GAMEPAD_Y):
				sound1.play();
				thing->buildBlockUp();
			break;

			case (XINPUT_GAMEPAD_B):
				sound1.play();
				thing->buildBlockRight();
			break;
		}
	}
}


//--------------------------------------------------------------------------------------
// Render the help and statistics text. This function uses the ID3DXFont interface for 
// efficient text rendering.
//--------------------------------------------------------------------------------------
void RenderText()
{
	
    g_pTxtHelper->Begin();
    g_pTxtHelper->SetInsertionPos( 5, 5 );
    g_pTxtHelper->SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 0.0f, 1.0f ) );
	WCHAR strBufferLevel[50];
	swprintf(strBufferLevel, L"Level: %d", levels->currentLevel+1);
	g_pTxtHelper->DrawTextLine(strBufferLevel);


	WCHAR strBufferBlocks[50];
	swprintf(strBufferBlocks, L"Blocks Available: %d", thing->numberOfBlocks);
	g_pTxtHelper->DrawTextLine(strBufferBlocks);
	
	WCHAR strBufferTime[50];
	
	int t = timeElapsed;

	swprintf(strBufferTime, L"Seconds Elapsed: %d", t);

	g_pTxtHelper->DrawTextLine(strBufferTime);

	WCHAR strBuffer[50];
	int numControllers = 0;
	for (int i = 0; i < MAX_CONTROLLERS; i++)
		if (g_Controllers[i].connected) numControllers++;
	swprintf(strBuffer, L"Num Controllers found: %d", numControllers);

    g_pTxtHelper->End();
	
}


//--------------------------------------------------------------------------------------
// Reject any D3D10 devices that aren't acceptable by returning false
//--------------------------------------------------------------------------------------
bool CALLBACK IsD3D10DeviceAcceptable( UINT Adapter, UINT Output, D3D10_DRIVER_TYPE DeviceType,
                                       DXGI_FORMAT BackBufferFormat, bool bWindowed, void* pUserContext )
{
    return true;
}


//--------------------------------------------------------------------------------------
// Create any D3D10 resources that aren't dependant on the back buffer
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D10CreateDevice( ID3D10Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc,
                                      void* pUserContext )
{
    HRESULT hr;

    V_RETURN( D3DX10CreateSprite( pd3dDevice, 500, &g_pSprite10 ) );
    V_RETURN( g_DialogResourceManager.OnD3D10CreateDevice( pd3dDevice ) );
    V_RETURN( g_SettingsDlg.OnD3D10CreateDevice( pd3dDevice ) );
    V_RETURN( D3DX10CreateFont( pd3dDevice, 15, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET,
                                OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
                                L"Arial", &g_pFont10 ) );
    g_pTxtHelper = new CDXUTTextHelper( NULL, NULL, g_pFont10, g_pSprite10, 15 );



	//**********************************************************************//
	// We use the camera object to store the View and Projection matrices.  //
	// The projection matrix is created in OnD3D10ResizedSwapChain().		//
	// Setup the camera's view parameters here, which creates the view		//
	// matrix.																//
	//**********************************************************************//

	setStaticCameraPosition();


	//**********************************************************************//
	// Create a new thing and create a shader effect and a mesh to go with  //
	// it.																	//
	//**********************************************************************//
	
	g_p_Effect = new EffectForAbstractThing3D(pd3dDevice, L"Thing3D.fx");

	g_p_Effect->lightDirection		 = D3DXVECTOR4(0, 1, -1, 1);
	g_p_Effect->lightDiffuseColour	 = D3DXVECTOR4(1, 1, 1, 1);
	g_p_Effect->lightAmbientColour	 = D3DXVECTOR4(0.3f, 0.3f, 0.3f, 0.3f);
	


	//**********************************************************************//
	// Create a new thing and plug the shader effect and various other stuff//
	// into it.																//
	//**********************************************************************//
	box      = new Thing3D(pd3dDevice);
	box2 = new Thing3D(pd3dDevice);
	thing      = new Ball(pd3dDevice, true);
	box2->setEffect(g_p_Effect);
	box2->createMesh(L"Media\\CloudBox\\skysphere.sdkmesh");
	box2->setScale(0.05f,0.05f, 0.05f);

	thing->setEffect(g_p_Effect);
	thing->createMesh(L"Media\\Sphere\\Sphere.sdkmesh");
	thing->setInitialDirection(0, 0, -1);
	thing->setScale(0.05, 0.05, 0.05);
	thing->moveTo(6, 16, 25);

	

	
	
	box->setEffect(g_p_Effect);
	box->createMesh(L"Media\\CloudBox\\skysphere.sdkmesh");
	box->setScale(0.05f,0.05f, 0.05f);
	
	levels = new levelLoader(pd3dDevice, g_p_Effect);
	levels->loadLevel(thing);
	
	V(newSoundManager.init());
	V(sound1.createSoundFromFile(L"Media\\Sound\\Clang.wav", 1));
	
	sound2.numAudioChannels = 1;		//I'm afraid you have to guess these
	sound2.bitsPerSample = 8;
	sound2.samplingRate = 20000;
	V(sound2.createSoundFromFile(L"Media\\Sound\\Boing.wav", 1));
	
	sound3.numAudioChannels = 1;		//I'm afraid you have to guess these
	sound3.bitsPerSample = 8;
	sound3.samplingRate = 24000;

	V(sound3.createSoundFromFile(L"Media\\Sound\\Horse.wav", 1));

	return S_OK;
}






//**************************************************************************//
// Create any D3D10 resources that depend on the back buffer.  This			//
// includes creating the projection matrix, which is stored in the camera.	//
//**************************************************************************//

HRESULT CALLBACK OnD3D10ResizedSwapChain( ID3D10Device* pd3dDevice, IDXGISwapChain* pSwapChain,
                                          const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
    HRESULT hr;

    V_RETURN( g_DialogResourceManager.OnD3D10ResizedSwapChain( pd3dDevice, pBackBufferSurfaceDesc ) );
    V_RETURN( g_SettingsDlg.OnD3D10ResizedSwapChain( pd3dDevice, pBackBufferSurfaceDesc ) );




	//******************************************************************//
	// Setup the camera's projection parameters.  The camera now		//
	// stores the projection matrix.									//
	//******************************************************************//

    float fAspectRatio = pBackBufferSurfaceDesc->Width / ( FLOAT )pBackBufferSurfaceDesc->Height;
    g_Camera.SetProjParams( D3DX_PI / 4, fAspectRatio, 0.1f, 1000.0f );
    g_Camera.SetWindow( pBackBufferSurfaceDesc->Width, pBackBufferSurfaceDesc->Height );
    g_Camera.SetButtonMasks( MOUSE_LEFT_BUTTON, MOUSE_WHEEL, MOUSE_MIDDLE_BUTTON );

    
	
	g_HUD.SetLocation( pBackBufferSurfaceDesc->Width - 170, 0 );
    g_HUD.SetSize( 170, 170 );
    g_SampleUI.SetLocation( pBackBufferSurfaceDesc->Width - 170, pBackBufferSurfaceDesc->Height - 300 );
    g_SampleUI.SetSize( 170, 300 );

    return S_OK;
}






//**************************************************************************//
// This one is called sfom several places and set's the camera's position	//
// back to its default.														//
//**************************************************************************//

void setStaticCameraPosition()
{
	//**********************************************************************//
	// We use the camera object to store the View and Projection matrices.  //
	// The projection matrix is created in OnD3D10ResizedSwapChain().		//
	// Setup the camera's view parameters here, which creates the view		//
	// matrix.																//
	//**********************************************************************//

    D3DXVECTOR3 vecEye( 30.0f, 40.0f, -25.0f );
    D3DXVECTOR3 vecAt ( 30.0f, 12.5f, 24.0f );
    g_Camera.SetViewParams( &vecEye, &vecAt );
}



int counter = 0;

//**************************************************************************//
// Render the scene using the D3D10 device.									//
// This looks very different to Dx9, in particular the old beginScene() and //
// endScene() are abscent.													//
//**************************************************************************//

int zLookAt = 1;
int xLookAt = 1;

void CALLBACK OnD3D10FrameRender( ID3D10Device* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext )
{
	
		D3DXMATRIX matWorld;
		D3DXMATRIX matView;
		D3DXMATRIX matProjection;

		timeElapsed = fTime;

		float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		ID3D10RenderTargetView* pRTV = DXUTGetD3D10RenderTargetView();
		pd3dDevice->ClearRenderTargetView( pRTV, ClearColor );

		// Clear the depth stencil
		ID3D10DepthStencilView* pDSV = DXUTGetD3D10DepthStencilView();
		pd3dDevice->ClearDepthStencilView( pDSV, D3D10_CLEAR_DEPTH, 1.0, 0 );

		// If the settings dialog is being shown, then render it instead of rendering the app's scene
		if( g_SettingsDlg.IsActive() )
		{
			g_SettingsDlg.OnRender( fElapsedTime );
			return;
		}



  		//**********************************************************************//
		// Get the projection & view matrix, which are now hidden in the the	//
		// camera class.														//
		//**********************************************************************//

		matProjection = *g_Camera.GetProjMatrix();
		matView       = *g_Camera.GetViewMatrix();
   

  		//**********************************************************************//
		// Pass the light direction and colour to the shader.  It seems silly   //
		// to do this every frame, but I suppose the light might have moved..	//
		//**********************************************************************//

		g_p_Effect->pLightDirectionInShader->SetFloatVector(       (float *) &g_p_Effect->lightDirection);
		g_p_Effect->pLightDiffuseColourInShader->SetFloatVector(   (float *) &g_p_Effect->lightDiffuseColour);
		g_p_Effect->pLightAmbientColourInShader->SetFloatVector(   (float *) &g_p_Effect->lightAmbientColour);
		if (counter == 1)
		{
			
			levels->nextLevel();
			levels->loadLevel(thing);
			counter = 0;
		}

		if (levels->levels.at(levels->currentLevel)->levelFinished)
		{
			counter = 1;
			stopVibrating = true;
		}
	
		if (levels->levels.at(levels->currentLevel)->levelFinished == false && levels->levels.at(levels->currentLevel)->loading == false)

		{
			stopVibrating = false;
			//**********************************************************************//
			// Render the thing3D.													//
			// Note, the Thing3D creates its own world matrix.  But it needs to know//
			// the other matrices to pass to its own shader.						//
			//**********************************************************************//

			thing->matView       = matView;
			thing->matProjection = matProjection;

	

			box2->rotateBy(0, 0.01, 0);
			box2->matView       = matView;
			box2->matProjection = matProjection;


			box2->render();
	
	
			//**********************************************************************//
			// Render the thing if the camera isn't on it, otherwise get a tiger's	//
			// eye view of the world.												//
			//**********************************************************************//

			if (g_b_WorldCamera) 
			{
				
			}
			else if (g_b_NonMoveCamera)
			{
			
					D3DXVECTOR3 viewerPos;
					D3DXVECTOR3 lookAtThis;
					D3DXVECTOR3 up         ( 0.0f, 1.0f, 0.0f );
					D3DXVECTOR3 newUp;
					D3DXMATRIX matView;

					//Set the viewer's position to the position of the thing.


					D3DXVECTOR3 newDir, lookAtPoint, tempView;

					viewerPos.x = thing->x;   
					viewerPos.y = thing->y ;
					viewerPos.z = thing->z ;
					D3DXMATRIX matScale;
					D3DXMatrixScaling(&matScale, 0.5, 0.5, 0.5);
					D3DXMATRIX matTry = thing->matTranslate * matScale;

					 D3DXVec3TransformCoord(&tempView, &viewerPos, &matTry);
		
			
			
					D3DXVec3TransformCoord(&newDir, &thing->initVecDir, &thing->matWorld);

					thing->setUpWorldMatrix();

					D3DXVec3Normalize(&lookAtPoint, &newDir);
					if ( thing->xSpeed > 0)
					{
						xLookAt = 1;
					}
					else
					{
						xLookAt = -1;
					}

					if ( xLookAt == 1)
					{
						lookAtPoint.x += thing->x + 1;
					}
					else if (xLookAt == -1)
					{
						lookAtPoint.x += thing->x + 1;
					}
					lookAtPoint.y += thing->y;

					if ( thing->zSpeed > 0)
					{
						zLookAt = 1;
					}
					else
					{
						zLookAt = -1;
					}

					if ( zLookAt == 1)
					{
						lookAtPoint.z += thing->z + 1;
					}
					else if (zLookAt == -1)
					{
						lookAtPoint.z += thing->z + 1;
					}


					g_Camera.SetViewParams(&tempView, &lookAtPoint);
			
			}
			else if (g_b_thirdPersonCamera)
			{
					D3DXVECTOR3 viewerPos;
					D3DXVECTOR3 lookAtThis;
					D3DXVECTOR3 up         ( 0.0f, 1.0f, 0.0f );
					D3DXVECTOR3 newUp;
					D3DXMATRIX matView;

					//Set the viewer's position to the position of the thing.


					D3DXVECTOR3 newDir, lookAtPoint, tempView;

					viewerPos.x = thing->x;   
					viewerPos.y = thing->y+20 ;
					viewerPos.z = thing->z-8 ;
					D3DXMATRIX matScale;
					D3DXMatrixScaling(&matScale, 0.5, 0.5, 0.5);
					D3DXMATRIX matTry = thing->matTranslate * matScale;

					 D3DXVec3TransformCoord(&tempView, &viewerPos, &matTry);
		
			
			
					D3DXVec3TransformCoord(&newDir, &thing->initVecDir, &thing->matWorld);

					thing->setUpWorldMatrix();

					D3DXVec3Normalize(&lookAtPoint, &newDir);
					lookAtPoint.x += thing->x;
					lookAtPoint.y += thing->y;
					lookAtPoint.z += thing->z;

					g_Camera.SetViewParams(&tempView, &lookAtPoint);
			
					
			}
	
			
	
			
			levels->renderLevel(matView, matProjection);
		}
		else
		{
			float ClearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
			ID3D10RenderTargetView* pRTV = DXUTGetD3D10RenderTargetView();
			pd3dDevice->ClearRenderTargetView( pRTV, ClearColor );
			ID3D10DepthStencilView* pDSV = DXUTGetD3D10DepthStencilView();
			pd3dDevice->ClearDepthStencilView( pDSV, D3D10_CLEAR_DEPTH, 1.0, 0 );

			 g_pTxtHelper->Begin();
			 g_pTxtHelper->SetInsertionPos( 300, 400 );
			g_pTxtHelper->SetForegroundColor( D3DXCOLOR( 1.0f, 0.0f, 0.0f, 1.0f ) );
			WCHAR strBufferLevel[50];
			swprintf(strBufferLevel, L"Loading Level: %d", levels->currentLevel+2);
			g_pTxtHelper->DrawTextLine(strBufferLevel);

			g_pTxtHelper->End();
			//
		}
		if (!g_b_NonMoveCamera)
		{
			thing->render();
		}

		sound1.updateStatus();
		sound2.updateStatus();
		sound3.updateStatus();
		sound1.pXACTWave->GetState(&sound1State);
	
		//****************************************************************************//
		// And finally, render the DXUT buttons and the thing which grandly calls	  //
		// itself a HUD (Head Up Display).											  //
		//****************************************************************************//
		if (counter == 0)
		{
			DXUT_BeginPerfEvent( DXUT_PERFEVENTCOLOR, L"HUD / Stats" );
		
			RenderText();
			g_HUD.OnRender( fElapsedTime );
			g_SampleUI.OnRender( fElapsedTime );
			DXUT_EndPerfEvent();
		}

	}
	




//--------------------------------------------------------------------------------------
// Release D3D10 resources created in OnD3D10ResizedSwapChain 
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D10ReleasingSwapChain( void* pUserContext )
{
    g_DialogResourceManager.OnD3D10ReleasingSwapChain();
}


//--------------------------------------------------------------------------------------
// Release D3D10 resources created in OnD3D10CreateDevice 
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D10DestroyDevice( void* pUserContext )
{
    g_DialogResourceManager.OnD3D10DestroyDevice();
    g_SettingsDlg.OnD3D10DestroyDevice();
    SAFE_RELEASE( g_pFont10 );
    SAFE_DELETE( g_p_Effect );
    SAFE_RELEASE( g_pVertexLayout );
    SAFE_RELEASE( g_pSprite10 );
    SAFE_DELETE( g_pTxtHelper );

	SAFE_DELETE(thing);
	SAFE_DELETE(levels);
	SAFE_DELETE(box);
	SAFE_DELETE(box2);
}


//--------------------------------------------------------------------------------------
// Called right before creating a D3D9 or D3D10 device, allowing the app to modify the device settings as needed
//--------------------------------------------------------------------------------------
bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, void* pUserContext )
{
    if( pDeviceSettings->ver == DXUT_D3D9_DEVICE )
    {
        IDirect3D9* pD3D = DXUTGetD3D9Object();
        D3DCAPS9 Caps;
        pD3D->GetDeviceCaps( pDeviceSettings->d3d9.AdapterOrdinal, pDeviceSettings->d3d9.DeviceType, &Caps );

        // If device doesn't support HW T&L or doesn't support 1.1 vertex shaders in HW 
        // then switch to SWVP.
        if( ( Caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT ) == 0 ||
            Caps.VertexShaderVersion < D3DVS_VERSION( 1, 1 ) )
        {
            pDeviceSettings->d3d9.BehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
        }

        // Debugging vertex shaders requires either REF or software vertex processing 
        // and debugging pixel shaders requires REF.  
#ifdef DEBUG_VS
        if( pDeviceSettings->d3d9.DeviceType != D3DDEVTYPE_REF )
        {
            pDeviceSettings->d3d9.BehaviorFlags &= ~D3DCREATE_HARDWARE_VERTEXPROCESSING;
            pDeviceSettings->d3d9.BehaviorFlags &= ~D3DCREATE_PUREDEVICE;
            pDeviceSettings->d3d9.BehaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
        }
#endif
#ifdef DEBUG_PS
        pDeviceSettings->d3d9.DeviceType = D3DDEVTYPE_REF;
#endif
    }

    // For the first device created if its a REF device, optionally display a warning dialog box
    static bool s_bFirstTime = true;
    if( s_bFirstTime )
    {
        s_bFirstTime = false;
        if( ( DXUT_D3D9_DEVICE == pDeviceSettings->ver && pDeviceSettings->d3d9.DeviceType == D3DDEVTYPE_REF ) ||
            ( DXUT_D3D10_DEVICE == pDeviceSettings->ver &&
              pDeviceSettings->d3d10.DriverType == D3D10_DRIVER_TYPE_REFERENCE ) )
            DXUTDisplaySwitchingToREFWarning( pDeviceSettings->ver );
    }

    return true;
}





//**************************************************************************//
// Handle updates to the scene.  This is called regardless of which D3D API //
// is used.																	//
//**************************************************************************//

void CALLBACK OnFrameMove( double fTime, float fElapsedTime, void* pUserContext )
{
    // Update the camera's position based on user input 

    
//	g_Camera.FrameMove( fElapsedTime );

	UpdateControllerState(fElapsedTime);

	if (g_b_LeftArrowDown)  thing->xSpeed -= fElapsedTime*4.0f;
	if (g_b_RightArrowDown) thing->xSpeed += fElapsedTime*4.0f;

	if (g_b_UpArrowDown)   thing->zSpeed += fElapsedTime*4.0f;
	if (g_b_DownArrowDown) thing->zSpeed -= fElapsedTime*4.0f;

	if (g_b_SpaceDown) 
	{
 		sound2.play();
		thing->jump();
		
	}

	if (g_b_BDown) thing->destroyBlock();

	if (g_b_NDown)
	{
		sound1.play();
		thing->buildBlock();
	}

	
	
	//**********************************************************************//
	// Get the thing to do its thing, as it were.							//
	//**********************************************************************//

	thing->frameTime = fElapsedTime;
	thing->doBehaviour();


	
}


//--------------------------------------------------------------------------------------
// Handle messages to the application
//--------------------------------------------------------------------------------------
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool* pbNoFurtherProcessing,
                          void* pUserContext )
{
    // Pass messages to dialog resource manager calls so GUI state is updated correctly
    *pbNoFurtherProcessing = g_DialogResourceManager.MsgProc( hWnd, uMsg, wParam, lParam );
    if( *pbNoFurtherProcessing )
        return 0;

    // Pass messages to settings dialog if its active
    if( g_SettingsDlg.IsActive() )
    {
        g_SettingsDlg.MsgProc( hWnd, uMsg, wParam, lParam );
        return 0;
    }

    // Give the dialogs a chance to handle the message first
    *pbNoFurtherProcessing = g_HUD.MsgProc( hWnd, uMsg, wParam, lParam );
    if( *pbNoFurtherProcessing )
        return 0;
    *pbNoFurtherProcessing = g_SampleUI.MsgProc( hWnd, uMsg, wParam, lParam );
    if( *pbNoFurtherProcessing )
        return 0;

    // Pass all remaining windows messages to camera so it can respond to user input
    g_Camera.HandleMessages( hWnd, uMsg, wParam, lParam );

    return 0;
}


//**************************************************************************//
// Handle key events.														//
//**************************************************************************//

void CALLBACK OnKeyboard( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext )
{
    switch( nChar )
    {


		HRESULT hr;

		case VK_LEFT:  g_b_LeftArrowDown  = bKeyDown; break;
		case VK_RIGHT: g_b_RightArrowDown = bKeyDown; break;
		case VK_UP:    g_b_UpArrowDown    = bKeyDown; break;
		case VK_DOWN:  g_b_DownArrowDown  = bKeyDown; break;
		case VK_SPACE:  g_b_SpaceDown  = bKeyDown; break;
		case 'b':        g_b_BDown  = bKeyDown; break;
		case 'B':        g_b_BDown  = bKeyDown; break;
		case 'n':        g_b_NDown  = bKeyDown; break;
		case 'N':        g_b_NDown  = bKeyDown; break;

		case 'C':	   
			{
				g_b_NonMoveCamera  = true;     
				g_b_thirdPersonCamera = false;
				g_b_WorldCamera = false;
				break;

			}
		case 'c':	   
			{
				g_b_NonMoveCamera  = true;     
				g_b_thirdPersonCamera = false;
				g_b_WorldCamera = false;
				break;

			}
 		
		case 'V':	   
			{
				
				g_b_NonMoveCamera  = false;     
				g_b_thirdPersonCamera = false;
				g_b_WorldCamera = true;
				setStaticCameraPosition(); 
				break;

			}
		case 'v':	
			{
				g_b_NonMoveCamera  = false;     
				g_b_thirdPersonCamera = false;
				g_b_WorldCamera = true;
				setStaticCameraPosition(); 
				break;

			}
			          
		case 'x':
			{
				g_b_NonMoveCamera  = false;     
				g_b_thirdPersonCamera = true;
				g_b_WorldCamera = false;
				break;

			}

		case 'X':

			{

				g_b_NonMoveCamera  = false;     
				g_b_thirdPersonCamera = true;
				g_b_WorldCamera = false;
				break;
			}
		

		
    }
}


//--------------------------------------------------------------------------------------
// Handles the GUI events
//--------------------------------------------------------------------------------------
void CALLBACK OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl* pControl, void* pUserContext )
{
    switch( nControlID )
    {
        case IDC_TOGGLEFULLSCREEN:
            DXUTToggleFullScreen(); break;
        case IDC_TOGGLEREF:
            DXUTToggleREF(); break;
        case IDC_TOGGLEWARP:
            DXUTToggleWARP(); break;
        case IDC_CHANGEDEVICE:
            g_SettingsDlg.SetActive( !g_SettingsDlg.IsActive() ); break;
    }
}

