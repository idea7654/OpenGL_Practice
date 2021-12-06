//--------------------------------------------------------------------------------------
// File: Tutorial07.cpp
//
// This application demonstrates texturing
//
// http://msdn.microsoft.com/en-us/library/windows/apps/ff729724.aspx
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License (MIT).
//--------------------------------------------------------------------------------------
#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>
//#include <directxcolors.h>
#include "DDSTextureLoader.h"


#define NUM_OBJECT	2

//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------

ID3D11DeviceContext*                g_pImmediateContext = nullptr;
IDXGISwapChain*                     g_pSwapChain = nullptr;

ID3D11RenderTargetView*             g_pRenderTargetView = nullptr;
ID3D11DepthStencilView*             g_pDepthStencilView = nullptr;


ID3D11VertexShader*                 g_pVertexShader[NUM_OBJECT] = {nullptr,};
ID3D11PixelShader*                  g_pPixelShader[NUM_OBJECT] = {nullptr,};

ID3D11InputLayout*                  g_pVertexLayout[NUM_OBJECT] = {nullptr,};

ID3D11Buffer*                       g_pVertexBuffer[NUM_OBJECT] = {nullptr,};
ID3D11Buffer*                       g_pIndexBuffer[NUM_OBJECT] = {nullptr,};


// Constant Buffer
ID3D11Buffer*                       g_pCB_InLoop = nullptr;


ID3D11ShaderResourceView*           g_pTextureRV = nullptr;
ID3D11SamplerState*                 g_pSamplerLinear = nullptr;




#include "Mesh.h"

HRESULT DX_Setup_V_Buffers_from_Mesh( int vKind , MESH* mesh ,
										ID3D11Buffer** pp_VertexBuffer, ID3D11Buffer** pp_IndexBuffer );

HRESULT DX_Setup_Shader_Program(	int vtx_Kind ,
									const WCHAR* filepath ,
									ID3D11VertexShader** pp_VertexShader , ID3D11PixelShader** pp_PixelShader , 
									ID3D11InputLayout** pp_VertexLayout  );

HRESULT DX_Initialise_Device_SwapChain_Context_ETC( HWND hWnd , 													
														ID3D11DeviceContext**	pp_ImmediateContext ,
														IDXGISwapChain**		pp_SwapChain,
	
														ID3D11RenderTargetView** pp_RenderTargetView ,
														ID3D11DepthStencilView** pp_DepthStencilView	);


HRESULT DX_Setup_Constant_Buffers( ID3D11Buffer** pp_CB );

HRESULT DX_Setup_Texture( const WCHAR* filename , ID3D11ShaderResourceView** pp_TextureRV , ID3D11SamplerState** pp_SamplerLinear );

void DX_Change_Aspect_Ratio( int width , int height );


int NUM_IDX[NUM_OBJECT] = {0,};
UINT g_Stride[NUM_OBJECT];
UINT g_Offset = 0;

UINT Size_VTX_PNT( void );
UINT Size_VTX_PC( void );

HRESULT InitDevice( HWND hWnd )
{
    HRESULT hr = S_OK;

    

	DX_Initialise_Device_SwapChain_Context_ETC( hWnd , 
												&g_pImmediateContext ,
												&g_pSwapChain,	
												&g_pRenderTargetView ,
												&g_pDepthStencilView	);



	

	MESH mesh[NUM_OBJECT];

	mesh[0].Load_OBJ( "./cpp/assets/teapot.obj" );
	mesh[1].Load_OBJ( "./cpp/assets/octahedron.obj" );

	int renderKind[2] = {332,34};

	g_Stride[0] = Size_VTX_PNT();
	g_Stride[1] = Size_VTX_PC();

	
	// for each object
	for( int i=0 ; i<NUM_OBJECT ; i++ )
	{
		NUM_IDX[i] = mesh[i].indices.size();

		// Inititalise Buffers from MESH
		DX_Setup_V_Buffers_from_Mesh( renderKind[i] , &mesh[i] , &g_pVertexBuffer[i], &g_pIndexBuffer[i] );
		if( FAILED(hr) )	return hr;

		// Inititalise Shaders from File & Init Layout
		hr = DX_Setup_Shader_Program( renderKind[i] , L"./cpp/dxPhong.fxh" , 
										&g_pVertexShader[i] , &g_pPixelShader[i] , &g_pVertexLayout[i] );
		if( FAILED(hr) )	return hr;
	}



	// Init Shader -- Const Buffers ( similar to Uniform in GL )
    hr = DX_Setup_Constant_Buffers( &g_pCB_InLoop );
	if( FAILED(hr) )	return hr;

	
    // Load the Texture (the target object is selected below...)
	hr = DX_Setup_Texture( L"./cpp/assets/DX_Power.dds" , &g_pTextureRV , &g_pSamplerLinear );
    if( FAILED( hr ) )
        return hr;



	// Set texture for the pixel shader #0 for the object #0 (This can be in Loop)
	g_pImmediateContext->PSSetShader( g_pPixelShader[0], nullptr, 0 );
		g_pImmediateContext->PSSetShaderResources( 0, 1, &g_pTextureRV );
		g_pImmediateContext->PSSetSamplers( 0, 1, &g_pSamplerLinear );



	
    // Set primitive topology
    g_pImmediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	
	

	// Currently Viewport is set once at initial width & height --> swap chain and others are already set by (w x h)
	RECT rc;    GetClientRect( hWnd, &rc );
    UINT width = rc.right - rc.left;
    UINT height = rc.bottom - rc.top;
	
	D3D11_VIEWPORT vp;
    vp.Width = (FLOAT)width;
    vp.Height = (FLOAT)height;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    g_pImmediateContext->RSSetViewports( 1, &vp );

	// ProjMat with AR
	DX_Change_Aspect_Ratio( width , height );


    return S_OK;
}





//--------------------------------------------------------------------------------------
// Render Loop
//--------------------------------------------------------------------------------------

float bgCol[4] = { 0.2f, 0.2f, 0.5f, 0.0f };

void DX_Control_Shader_CB( ID3D11DeviceContext* g_pImmediateContext , ID3D11Buffer* g_pCB );

void RenderScene_DX( void )
{
    
	// Update Matrix in Constant Buffers... can include matrices for all objects at once or use several CB's
	DX_Control_Shader_CB( g_pImmediateContext , g_pCB_InLoop );

	
    
    // Clear the back buffer
	g_pImmediateContext->ClearRenderTargetView( g_pRenderTargetView, bgCol );
	// Clear the depth buffer to 1.0 (max depth)    
	g_pImmediateContext->ClearDepthStencilView( g_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0 );


		for( int i=0 ; i<NUM_OBJECT ; i++ )
		{
			g_pImmediateContext->VSSetShader( g_pVertexShader[i], nullptr, 0 );
				g_pImmediateContext->VSSetConstantBuffers( 0, 1, &g_pCB_InLoop );	// slot 0, 1 buffer

			g_pImmediateContext->PSSetShader( g_pPixelShader[i], nullptr, 0 );
				g_pImmediateContext->PSSetConstantBuffers( 0, 1, &g_pCB_InLoop );

			// Set the vertex input layout (Input Assembly)
			g_pImmediateContext->IASetInputLayout( g_pVertexLayout[i] );

			 // Set vertex buffer		
			g_pImmediateContext->IASetVertexBuffers( 0, 1, &g_pVertexBuffer[i], &g_Stride[i], &g_Offset );

			// Set index buffer
			g_pImmediateContext->IASetIndexBuffer( g_pIndexBuffer[i], DXGI_FORMAT_R16_UINT, 0 );

			// Draw !!
			g_pImmediateContext->DrawIndexed( NUM_IDX[i], 0, 0 );
		}

    // Present our back buffer to our front buffer    
    g_pSwapChain->Present( 0, 0 );
}




//--------------------------------------------------------------------------------------
// Clean up the objects we've created
//--------------------------------------------------------------------------------------

void Clean_DX_Objects( void );

void Clean_DX()
{
    if( g_pImmediateContext ) g_pImmediateContext->ClearState();

    if( g_pSamplerLinear ) g_pSamplerLinear->Release();
    if( g_pTextureRV ) g_pTextureRV->Release();

   // if( g_pCBChangeOnResize ) g_pCBChangeOnResize->Release();
    if( g_pCB_InLoop ) g_pCB_InLoop->Release();

	for( int i=0 ; i<NUM_OBJECT ; i++ )
	{
		if( g_pVertexBuffer ) g_pVertexBuffer[i]->Release();
		if( g_pIndexBuffer ) g_pIndexBuffer[i]->Release();
		if( g_pVertexLayout ) g_pVertexLayout[i]->Release();

		if( g_pVertexShader ) g_pVertexShader[i]->Release();
		if( g_pPixelShader ) g_pPixelShader[i]->Release();
	}
	    
	
    if( g_pDepthStencilView ) g_pDepthStencilView->Release();
    
	if( g_pRenderTargetView ) g_pRenderTargetView->Release();
    
	
    if( g_pSwapChain ) g_pSwapChain->Release();
    
	
    if( g_pImmediateContext ) g_pImmediateContext->Release();
    

	Clean_DX_Objects( );
}




//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
bool Init_DX( HWND hWnd )
{

    if( FAILED( InitDevice( hWnd ) ) )
    {
        Clean_DX();
        return 0;
    }

 	return 1;
}







/*
    // Compile the vertex shader
    ID3DBlob* pVSBlob = nullptr;
    hr = CompileShaderFromFile( L"./cpp/Tutorial07.fxh", "VS", "vs_4_0", &pVSBlob );
    if( FAILED( hr ) )
    {
        MessageBox( nullptr,
                    L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK );
        return hr;
    }

    // Create the vertex shader
    hr = g_pd3dDevice->CreateVertexShader( pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &g_pVertexShader );
    if( FAILED( hr ) )
    {    
        pVSBlob->Release();
        return hr;
    }

   

    // Compile the pixel shader
    ID3DBlob* pPSBlob = nullptr;
    hr = CompileShaderFromFile( L"./cpp/Tutorial07.fxh", "PS", "ps_4_0", &pPSBlob );
    if( FAILED( hr ) )
    {
        MessageBox( nullptr,
                    L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK );
        return hr;
    }

    // Create the pixel shader
    hr = g_pd3dDevice->CreatePixelShader( pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &g_pPixelShader );
    pPSBlob->Release();
    if( FAILED( hr ) )
        return hr;



	 // Define the input layout
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    UINT numElements = ARRAYSIZE( layout );

    // Create the input layout
    hr = g_pd3dDevice->CreateInputLayout( layout, numElements, pVSBlob->GetBufferPointer(),
                                          pVSBlob->GetBufferSize(), &g_pVertexLayout );
    pVSBlob->Release();
    if( FAILED( hr ) )
        return hr;

*/




/*    // Create vertex buffer
    SimpleVertex vertices[] =
    {
        { XMFLOAT3( -1.0f, 1.0f, -1.0f ), XMFLOAT2( 1.0f, 0.0f ) },
        { XMFLOAT3( 1.0f, 1.0f, -1.0f ), XMFLOAT2( 0.0f, 0.0f ) },
        { XMFLOAT3( 1.0f, 1.0f, 1.0f ), XMFLOAT2( 0.0f, 1.0f ) },
        { XMFLOAT3( -1.0f, 1.0f, 1.0f ), XMFLOAT2( 1.0f, 1.0f ) },

        { XMFLOAT3( -1.0f, -1.0f, -1.0f ), XMFLOAT2( 0.0f, 0.0f ) },
        { XMFLOAT3( 1.0f, -1.0f, -1.0f ), XMFLOAT2( 1.0f, 0.0f ) },
        { XMFLOAT3( 1.0f, -1.0f, 1.0f ), XMFLOAT2( 1.0f, 1.0f ) },
        { XMFLOAT3( -1.0f, -1.0f, 1.0f ), XMFLOAT2( 0.0f, 1.0f ) },

        { XMFLOAT3( -1.0f, -1.0f, 1.0f ), XMFLOAT2( 0.0f, 1.0f ) },
        { XMFLOAT3( -1.0f, -1.0f, -1.0f ), XMFLOAT2( 1.0f, 1.0f ) },
        { XMFLOAT3( -1.0f, 1.0f, -1.0f ), XMFLOAT2( 1.0f, 0.0f ) },
        { XMFLOAT3( -1.0f, 1.0f, 1.0f ), XMFLOAT2( 0.0f, 0.0f ) },

        { XMFLOAT3( 1.0f, -1.0f, 1.0f ), XMFLOAT2( 1.0f, 1.0f ) },
        { XMFLOAT3( 1.0f, -1.0f, -1.0f ), XMFLOAT2( 0.0f, 1.0f ) },
        { XMFLOAT3( 1.0f, 1.0f, -1.0f ), XMFLOAT2( 0.0f, 0.0f ) },
        { XMFLOAT3( 1.0f, 1.0f, 1.0f ), XMFLOAT2( 1.0f, 0.0f ) },

        { XMFLOAT3( -1.0f, -1.0f, -1.0f ), XMFLOAT2( 0.0f, 1.0f ) },
        { XMFLOAT3( 1.0f, -1.0f, -1.0f ), XMFLOAT2( 1.0f, 1.0f ) },
        { XMFLOAT3( 1.0f, 1.0f, -1.0f ), XMFLOAT2( 1.0f, 0.0f ) },
        { XMFLOAT3( -1.0f, 1.0f, -1.0f ), XMFLOAT2( 0.0f, 0.0f ) },

        { XMFLOAT3( -1.0f, -1.0f, 1.0f ), XMFLOAT2( 1.0f, 1.0f ) },
        { XMFLOAT3( 1.0f, -1.0f, 1.0f ), XMFLOAT2( 0.0f, 1.0f ) },
        { XMFLOAT3( 1.0f, 1.0f, 1.0f ), XMFLOAT2( 0.0f, 0.0f ) },
        { XMFLOAT3( -1.0f, 1.0f, 1.0f ), XMFLOAT2( 1.0f, 0.0f ) },
    };

    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof( SimpleVertex ) * 24;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA InitData = {};
    InitData.pSysMem = vertices;
    hr = g_pd3dDevice->CreateBuffer( &bd, &InitData, &g_pVertexBuffer );
    if( FAILED( hr ) )
        return hr;



    // Create index buffer
    // Create vertex buffer
    WORD indices[] =
    {
        3,1,0,
        2,1,3,

        6,4,5,
        7,4,6,

        11,9,8,
        10,9,11,

        14,12,13,
        15,12,14,

        19,17,16,
        18,17,19,

        22,20,21,
        23,20,22
    };

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof( WORD ) * 36;
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;
    InitData.pSysMem = indices;
    hr = g_pd3dDevice->CreateBuffer( &bd, &InitData, &g_pIndexBuffer );
    if( FAILED( hr ) )
        return hr;

		*/
