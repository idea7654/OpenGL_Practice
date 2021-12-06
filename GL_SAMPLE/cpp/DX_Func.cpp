
#define D3D11_VIDEO_NO_HELPERS

#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>

#include <stdio.h>






// Exist through lifetime but created only at Initialisation
ID3D11Device*		g_pd3dDevice	= nullptr;
ID3D11Texture2D*	g_pDepthStencil = nullptr;



//--------------------------------------------------------------------------------------
// Helper for compiling shaders with D3DCompile
//
// With VS 11, we could load up prebuilt .cso files instead...
//--------------------------------------------------------------------------------------
//HRESULT CompileShaderFromFile( const WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut )
HRESULT CompileShaderFromFile( const WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut )
{
	printf( "Shader: %s token compile ---> " , szEntryPoint );


    HRESULT hr = S_OK;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    dwShaderFlags |= D3DCOMPILE_DEBUG;

    // Disable optimizations to further improve shader debugging
    dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

    ID3DBlob* pErrorBlob = nullptr;
    hr = D3DCompileFromFile( szFileName, nullptr, nullptr, szEntryPoint, szShaderModel, 
        dwShaderFlags, 0, ppBlobOut, &pErrorBlob );
    if( FAILED(hr) )
    {
        if( pErrorBlob )
        {
			const char* msg = reinterpret_cast<const char*>( pErrorBlob->GetBufferPointer() );
			OutputDebugStringA( msg );

			printf( msg );

           // OutputDebugStringA( reinterpret_cast<const char*>( pErrorBlob->GetBufferPointer() ) );
            pErrorBlob->Release();
        }
        return hr;
    }
    if( pErrorBlob ) pErrorBlob->Release();


	printf( "done.\n\n" );

    return S_OK;
}






HRESULT DX_Setup_Shader_Program(	int vtx_Kind ,
									const WCHAR* filepath ,
									ID3D11VertexShader** pp_VertexShader , ID3D11PixelShader** pp_PixelShader , 
									ID3D11InputLayout** pp_VertexLayout  )
{
	HRESULT hr = S_OK;


	// Shader function names --> Prepare two vtx types with layouts
	const char *vsName = "";
	const char *psName = "";
	

	switch( vtx_Kind )
	{
		case 332:		vsName = "VS";		psName = "PS";		break;
		case 34:		vsName = "VS_PC";	psName = "PS_PC";	break;
	}
	
	D3D11_INPUT_ELEMENT_DESC layout_pnt[] =
	{
		{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,	0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",		0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",	0, DXGI_FORMAT_R32G32_FLOAT,	0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	D3D11_INPUT_ELEMENT_DESC layout_pc[] =
	{
		{ "POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,	0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR",		0, DXGI_FORMAT_R32G32B32_FLOAT,	0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};


	


	// Compile the vertex shader
    ID3DBlob* pVSBlob = nullptr;	// compiled shader binaries
    hr = CompileShaderFromFile( filepath , vsName, "vs_5_0", &pVSBlob );
    if( FAILED( hr ) )
    {
        MessageBox( nullptr,
                    L"The Shader file cannot be compiled.", L"Error", MB_OK );
        return hr;
    }

    // Create the vertex shader
    hr = g_pd3dDevice->CreateVertexShader( pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, pp_VertexShader );

    if( FAILED( hr ) ){	pVSBlob->Release();	return hr;	}




	switch( vtx_Kind )
	{
		UINT numElements;

		case 332:
				
			numElements = ARRAYSIZE( layout_pnt );

			// Create the input layout
			hr = g_pd3dDevice->CreateInputLayout( layout_pnt, numElements, pVSBlob->GetBufferPointer(),
												  pVSBlob->GetBufferSize(), pp_VertexLayout );
		break;


		case 34:		

			numElements = ARRAYSIZE( layout_pc );

			// Create the input layout
			hr = g_pd3dDevice->CreateInputLayout( layout_pc, numElements, pVSBlob->GetBufferPointer(),
												  pVSBlob->GetBufferSize(), pp_VertexLayout );
		break;
	}
    
	
	pVSBlob->Release();
	if( FAILED( hr ) )	return hr;




    // Compile the pixel shader
    ID3DBlob* pPSBlob = nullptr;
    hr = CompileShaderFromFile( filepath , psName, "ps_5_0", &pPSBlob );
    if( FAILED( hr ) )
    {
        MessageBox( nullptr,
                    L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK );
        return hr;
    }

    // Create the pixel shader
    hr = g_pd3dDevice->CreatePixelShader( pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, pp_PixelShader );
    pPSBlob->Release();
    if( FAILED( hr ) )
        return hr;

	
	return hr;

}




#include <directxmath.h>

#include "Mesh.h"

using namespace DirectX;

struct VTX_PNT{    XMFLOAT3 pos;	XMFLOAT3 norm;    XMFLOAT2 tex;	};
struct VTX_PC {    XMFLOAT3 pos;	XMFLOAT4 col;	};	// or XMCOLOR

UINT Size_VTX_PNT( void ){	return sizeof(VTX_PNT);	}
UINT Size_VTX_PC( void ){	return sizeof(VTX_PC);	}

HRESULT DX_Setup_V_Buffers_from_Mesh( int vtx_Kind , MESH* mesh , 
										ID3D11Buffer** pp_VertexBuffer, ID3D11Buffer** pp_IndexBuffer )
{

	HRESULT hr = S_OK;


	D3D11_BUFFER_DESC bd = {};
	D3D11_SUBRESOURCE_DATA InitData = {};


	int NUM_VTX = mesh->vertices.size();


	VTX_PNT* vtx_pnt = nullptr;
	VTX_PC*  vtx_pc  = nullptr;

	switch( vtx_Kind )
	{
		case 332:

			vtx_pnt = new VTX_PNT[NUM_VTX];

			for( int n=0 ; n<NUM_VTX ; n++ )
			{
				vtx_pnt[n].pos.x = mesh->vertices[n].Position.x;
				vtx_pnt[n].pos.y = mesh->vertices[n].Position.y;
				vtx_pnt[n].pos.z = -mesh->vertices[n].Position.z;	// changing to left-handed

				vtx_pnt[n].norm.x = mesh->vertices[n].Normal.x;
				vtx_pnt[n].norm.y = mesh->vertices[n].Normal.y;
				vtx_pnt[n].norm.z = -mesh->vertices[n].Normal.z;	// changing to left-handed

				vtx_pnt[n].tex.x = mesh->vertices[n].TexCoord.x;
				vtx_pnt[n].tex.y = mesh->vertices[n].TexCoord.y;
			}			
			
			bd.ByteWidth = sizeof( VTX_PNT ) * NUM_VTX;
			
			InitData.pSysMem = vtx_pnt;			

		break;

		case 34:

			vtx_pc = new VTX_PC[NUM_VTX];

			for( int n=0 ; n<NUM_VTX ; n++ )
			{
				vtx_pc[n].pos.x = mesh->vertices[n].Position.x;
				vtx_pc[n].pos.y = mesh->vertices[n].Position.y;
				vtx_pc[n].pos.z = -mesh->vertices[n].Position.z;	// changing to left-handed	

				vtx_pc[n].col.x = mesh->vertices[n].Color.r;
				vtx_pc[n].col.y = mesh->vertices[n].Color.g;
				vtx_pc[n].col.z = mesh->vertices[n].Color.b;
				vtx_pc[n].col.w = mesh->vertices[n].Color.a;
			}			
			
			bd.ByteWidth = sizeof( VTX_PC ) * NUM_VTX;
			
			InitData.pSysMem = vtx_pc;			

		break;

	}


	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	hr = g_pd3dDevice->CreateBuffer( &bd, &InitData, pp_VertexBuffer );
	if( FAILED( hr ) )	return hr;

	if( vtx_pnt )	delete [] vtx_pnt;
	if( vtx_pc  )	delete [] vtx_pc;

    



	// Create index buffer

	int NUM_IDX = mesh->indices.size();
	
	WORD* indices = new WORD[NUM_IDX];

	for( int i=0 ; i<NUM_IDX ; i++ )
	{
		indices[i] = (WORD)mesh->indices[i];
	}

	// changing to left-handed -- reverse culling for DX
	for( int i=0 ; i<NUM_IDX ; i+=3 )
	{
		WORD tmp = indices[i];
		indices[i] = indices[i+2];
		indices[i+2] = tmp;
	}

 
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof( WORD ) * NUM_IDX;
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;
    InitData.pSysMem = indices;
    hr = g_pd3dDevice->CreateBuffer( &bd, &InitData, pp_IndexBuffer );
    if( FAILED( hr ) )
        return hr;

	delete [] indices;


	return hr;
}








#include "DDSTextureLoader.h"

HRESULT DX_Setup_Texture( const WCHAR* filename , ID3D11ShaderResourceView** pp_TextureRV , ID3D11SamplerState** pp_SamplerLinear )
{
	HRESULT hr = S_OK;


	// Load the Texture
    hr = CreateDDSTextureFromFile( g_pd3dDevice, filename , nullptr, pp_TextureRV );
    if( FAILED( hr ) )
        return hr;

    // Create the sample state
    D3D11_SAMPLER_DESC sampDesc = {};
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    hr = g_pd3dDevice->CreateSamplerState( &sampDesc, pp_SamplerLinear );
    if( FAILED( hr ) )
        return hr;


	return hr;
}


// 16-byte align required ??
// Constant Buffer to be passed to Shaders --> Currently used for two kinds of shaders
struct CB_InLoop
{
	XMMATRIX MVP[2];	// for teapot and octahedron
	

	// below are for Phong Lighting (currently only for teapot)
	XMMATRIX worldMat;
	XMMATRIX litMat;

	XMFLOAT3 LightPos_World;
	XMFLOAT3 CameraPos_World;
};


HRESULT DX_Setup_Constant_Buffers( ID3D11Buffer** pp_CB )
{
	HRESULT hr = S_OK;

	// Create the constant buffers
	D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DEFAULT;    
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;

    bd.ByteWidth = sizeof(CB_InLoop);	// Inc. WorldMat, ViewMat, Color
    hr = g_pd3dDevice->CreateBuffer( &bd, nullptr, pp_CB );
    if( FAILED( hr ) )
        return hr;

	return hr;
}




////////////////////////////////// IN LOOP ///////////////////////////////////////////////////////////

XMMATRIX g_Projection;


// Used in GLFW resizing
void DX_Change_Aspect_Ratio( int width , int height )
{	
	float FoV = 3.141592f * 45.0f / 180.0f;
	float nearZ = 0.1f;
	float farZ = 200.0f;
	float AR = (float)width / (float)height;

	// Set the projection matrix --> BUT no change in framebuffer size (resolution)
    g_Projection = XMMatrixPerspectiveFovLH( FoV, AR, nearZ, farZ );
	
	// And no viewport update in DX
	
	printf( "Window Size (DX: not frame buffer size!) %dx%d\n" , width , height );
}



// Common from GLFW_UI.cpp
float* Get_Camera_Pos( void );

// only for DX
void Fill_Drag_Rotation( float f[16] );


float Get_DX_Time( void );

void DX_Control_Shader_CB( ID3D11DeviceContext* g_pImmediateContext , ID3D11Buffer* g_pCB )
{
	
	// Initialize the view matrix
	float* cam = Get_Camera_Pos();
    XMVECTOR Eye = XMVectorSet( cam[0], cam[1], -cam[2], 0.0f );
    XMVECTOR At =  XMVectorSet( 0.0f, 0.0f, 0.0f, 0.0f );
    XMVECTOR Up =  XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );
    
	XMMATRIX g_View = XMMatrixLookAtLH( Eye, At, Up );

    

	// MVP Matrix for teapot
	float rot[16];
	Fill_Drag_Rotation( rot );
	XMMATRIX wMat_teapot = XMMATRIX (rot);	// Come into Row based matrix

//	XMMATRIX sMat = XMMatrixScaling( 0.5f, 1.0f, 1.5f );
//	wMat_teapot = XMMatrixMultiply( sMat , wMat_teapot );
	
	XMMATRIX MVP_teapot = XMMatrixMultiply(   XMMatrixMultiply( wMat_teapot, g_View )  ,  g_Projection   );


	
	// get our time
    float time = Get_DX_Time( );

	// for octahedron
	XMMATRIX scaleMat = XMMatrixScaling( 0.1f, 0.2f, 0.1f );
	XMMATRIX rotMat = XMMatrixRotationAxis( XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f ) , -time  );
	XMMATRIX transMat = XMMatrixTranslation( 15.0f, 7.0f, 0.0f );
	
	XMMATRIX wMat_simple = XMMatrixMultiply(   XMMatrixMultiply( scaleMat, transMat )  ,  rotMat   );

	XMMATRIX MVP_simple = XMMatrixMultiply(   XMMatrixMultiply( wMat_simple, g_View )  ,  g_Projection   );
	
	
	// Light Pos at moving octahedron
	XMVECTOR lightPos = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	lightPos =  XMVector3Transform( lightPos , wMat_simple );




	// Fill Const Buffer
	CB_InLoop cbuf;

	cbuf.MVP[0] = XMMatrixTranspose( MVP_teapot );
	cbuf.MVP[1] = XMMatrixTranspose( MVP_simple );

	cbuf.worldMat	 = XMMatrixTranspose( wMat_teapot );

	XMVECTOR det = XMMatrixDeterminant(wMat_teapot);
	cbuf.litMat	 = XMMatrixInverse( &det , wMat_teapot );

	
	// Light pos world space
	XMStoreFloat3( &cbuf.LightPos_World , lightPos );

	// Camera pos world space
	cam = Get_Camera_Pos();
    cbuf.CameraPos_World = XMFLOAT3(cam[0], cam[1], -cam[2]);

	

	// Update Shader
	g_pImmediateContext->UpdateSubresource( g_pCB, 0, nullptr, &cbuf, 0, 0 );


}


/*
vector A = (x,y,z,w)

matrix M = { m00, m01, m02, m03,
			 m10, m11, m12, m13,
			 m20, m21, m22, m23,
			 m30, m31, m32, m33 }

B = A * M

B.x = A.x*m00 + A.y*m10 + A.z*m20 + a.w*m30;  -->  [A] dot [ColumnVector #0]
B.y = A.x*m01 + A.y*m11 + A.z*m21 + a.w*m31;
B.z = A.x*m02 + A.y*m12 + A.z*m22 + a.w*m32;
B.w = A.x*m03 + A.y*m13 + A.z*m23 + a.w*m33;

**[HLSL]**
mul(A,M) --> B.x = A dot RowVec #0 --> a[4] dot m[4]

*/




D3D_DRIVER_TYPE		g_driverType = D3D_DRIVER_TYPE_NULL;

float Get_DX_Time( void )
{
	// Update our time
    static float t = 0.0f;
    if( g_driverType == D3D_DRIVER_TYPE_REFERENCE )
    {
        t += ( float )XM_PI * 0.0125f;
    }
    else
    {
        static ULONGLONG timeStart = 0;
        ULONGLONG timeCur = GetTickCount64();
        if( timeStart == 0 )
            timeStart = timeCur;

		
        t = ( timeCur - timeStart ) / 1000.0f;
    }

	

	return t;
}




// Currently not used -- DX 11.1
ID3D11Device1*			g_pd3dDevice1 = nullptr;
ID3D11DeviceContext1*	g_pImmediateContext1 = nullptr;
IDXGISwapChain1*		g_pSwapChain1 = nullptr;


// Current DX ver
D3D_FEATURE_LEVEL	g_featureLevel = D3D_FEATURE_LEVEL_11_0;


HRESULT DX_Initialise_Device_SwapChain_Context_ETC( HWND hWnd , 

														ID3D11DeviceContext**	pp_ImmediateContext ,
														IDXGISwapChain**		pp_SwapChain,
	
														ID3D11RenderTargetView** pp_RenderTargetView ,
														ID3D11DepthStencilView** pp_DepthStencilView	)
{
	HRESULT hr = S_OK;


	RECT rc;
    GetClientRect( hWnd, &rc );
    UINT width = rc.right - rc.left;
    UINT height = rc.bottom - rc.top;

    UINT createDeviceFlags = 0;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };
    UINT numDriverTypes = ARRAYSIZE( driverTypes );

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };
    UINT numFeatureLevels = ARRAYSIZE( featureLevels );



    for( UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++ )
    {
        g_driverType = driverTypes[driverTypeIndex];
        hr = D3D11CreateDevice( nullptr, g_driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
                                D3D11_SDK_VERSION, &g_pd3dDevice, &g_featureLevel, pp_ImmediateContext );

        if ( hr == E_INVALIDARG )
        {
            // DirectX 11.0 platforms will not recognize D3D_FEATURE_LEVEL_11_1 so we need to retry without it
            hr = D3D11CreateDevice( nullptr, g_driverType, nullptr, createDeviceFlags, &featureLevels[1], numFeatureLevels - 1,
                                    D3D11_SDK_VERSION, &g_pd3dDevice, &g_featureLevel, pp_ImmediateContext );
        }

        if( SUCCEEDED( hr ) )
            break;
    }

    if( FAILED( hr ) )
        return hr;



    // Obtain DXGI factory from device (since we used nullptr for pAdapter above)
    IDXGIFactory1* dxgiFactory = nullptr;
    {
        IDXGIDevice* dxgiDevice = nullptr;
        hr = g_pd3dDevice->QueryInterface( __uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice) );
        if (SUCCEEDED(hr))
        {
            IDXGIAdapter* adapter = nullptr;
            hr = dxgiDevice->GetAdapter(&adapter);
            if (SUCCEEDED(hr))
            {
                hr = adapter->GetParent( __uuidof(IDXGIFactory1), reinterpret_cast<void**>(&dxgiFactory) );
                adapter->Release();
            }
            dxgiDevice->Release();
        }
    }
    if (FAILED(hr))
        return hr;



    // Create swap chain
    IDXGIFactory2* dxgiFactory2 = nullptr;
    hr = dxgiFactory->QueryInterface( __uuidof(IDXGIFactory2), reinterpret_cast<void**>(&dxgiFactory2) );
    if ( dxgiFactory2 )
    {
        // DirectX 11.1 or later
        hr = g_pd3dDevice->QueryInterface( __uuidof(ID3D11Device1), reinterpret_cast<void**>(&g_pd3dDevice1) );
        if (SUCCEEDED(hr))
        {
            (void) (*pp_ImmediateContext)->QueryInterface( __uuidof(ID3D11DeviceContext1), reinterpret_cast<void**>(&g_pImmediateContext1) );
        }

        DXGI_SWAP_CHAIN_DESC1 sd = {};
        sd.Width = width;
        sd.Height = height;
        sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.BufferCount = 1;

        hr = dxgiFactory2->CreateSwapChainForHwnd( g_pd3dDevice, hWnd, &sd, nullptr, nullptr, &g_pSwapChain1 );
        if (SUCCEEDED(hr))
        {
            hr = g_pSwapChain1->QueryInterface( __uuidof(IDXGISwapChain), reinterpret_cast<void**>(pp_SwapChain) );
        }

        dxgiFactory2->Release();
    }
    else
    {
        // DirectX 11.0 systems
        DXGI_SWAP_CHAIN_DESC sd = {};
        sd.BufferCount = 1;
        sd.BufferDesc.Width = width;
        sd.BufferDesc.Height = height;
        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.BufferDesc.RefreshRate.Numerator = 60;
        sd.BufferDesc.RefreshRate.Denominator = 1;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.OutputWindow = hWnd;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.Windowed = TRUE;

        hr = dxgiFactory->CreateSwapChain( g_pd3dDevice, &sd, pp_SwapChain );
    }

    // Note this tutorial doesn't handle full-screen swapchains so we block the ALT+ENTER shortcut
    dxgiFactory->MakeWindowAssociation( hWnd, DXGI_MWA_NO_ALT_ENTER );
    dxgiFactory->Release();

    if (FAILED(hr))
        return hr;


	// No flip in Wnd Mode. . .

    // Create a render target view
    ID3D11Texture2D* pBackBuffer = nullptr;
    hr = (*pp_SwapChain)->GetBuffer( 0, __uuidof( ID3D11Texture2D ), reinterpret_cast<void**>( &pBackBuffer ) );
    if( FAILED( hr ) )
        return hr;

    hr = g_pd3dDevice->CreateRenderTargetView( pBackBuffer, nullptr, pp_RenderTargetView );
    pBackBuffer->Release();
    if( FAILED( hr ) )
        return hr;


    // Create depth stencil texture
    D3D11_TEXTURE2D_DESC descDepth = {};
    descDepth.Width = width;
    descDepth.Height = height;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDepth.SampleDesc.Count = 1;
    descDepth.SampleDesc.Quality = 0;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    descDepth.CPUAccessFlags = 0;
    descDepth.MiscFlags = 0;
    
	hr = g_pd3dDevice->CreateTexture2D( &descDepth, nullptr, &g_pDepthStencil );
    if( FAILED( hr ) )
        return hr;



    // Create the depth stencil view
    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
    descDSV.Format = descDepth.Format;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;
    
	hr = g_pd3dDevice->CreateDepthStencilView( g_pDepthStencil, &descDSV, pp_DepthStencilView );
    if( FAILED( hr ) )
        return hr;



    (*pp_ImmediateContext)->OMSetRenderTargets( 1, pp_RenderTargetView, (*pp_DepthStencilView) );


	return hr;

}






////////// Clean Objects Here //////////////////

void Clean_DX_Objects( void )
{
	if( g_pDepthStencil ) g_pDepthStencil->Release();
	if( g_pSwapChain1 ) g_pSwapChain1->Release();
	if( g_pImmediateContext1 ) g_pImmediateContext1->Release();
	if( g_pd3dDevice1 ) g_pd3dDevice1->Release();
    if( g_pd3dDevice ) g_pd3dDevice->Release();
}






// Loop Change the color
// XMFLOAT4 g_vMeshColor( 0.7f, 0.7f, 0.7f, 1.0f );
// 
//    g_vMeshColor.x = ( sinf( t * 1.0f ) + 1.0f ) * 0.5f;
//    g_vMeshColor.y = ( cosf( t * 3.0f ) + 1.0f ) * 0.5f;
//    g_vMeshColor.z = ( sinf( t * 5.0f ) + 1.0f ) * 0.5f;

/*
    CBChangesEveryFrame cb;
	cb.mWorld = g_World;
	cb.mView  = XMMatrixTranspose( g_View );
    cb.vMeshColor = g_vMeshColor;
*/
