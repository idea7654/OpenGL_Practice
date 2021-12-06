

Texture2D txDiffuse : register( t0 );
SamplerState samLinear : register( s0 );


// Constant Buffer Variables (uniform in GL), b0~b7 ? slot 0
cbuffer cbChangesEveryFrame : register( b0 )
{
	matrix MVP[2];
	matrix worldMat;
	
	matrix litMat;	// for DX only --> inverse transpose of L matrix
	

	float3 LightPos_World;
	float3 CameraPos_World;
	
};


//--------------------------------------------------------------------------------------
struct VS_INPUT
{
    float4 pos_Model 	: POSITION;
	float3 norm_Model 	: NORMAL;
    float2 texCoord 	: TEXCOORD0;
};

struct PS_INPUT
{
    float4 pos_Clip : SV_POSITION;
    float2 UV 		: TEXCOORD0;
	
	float3 pos_World 		: POSITION0;
	float3 normVec_World 	: NORMAL1;
	float3 viewVec_World 	: NORMAL2;
	float3 lightVec_World 	: NORMAL3;
};



//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS( VS_INPUT vs )
{
	PS_INPUT ps = (PS_INPUT)0;
	
	vs.pos_Model.w = 1.0f;
	ps.pos_Clip = mul( vs.pos_Model, MVP[0] );	// Position in Clip-Space

	ps.UV = vs.texCoord;
	
	
	
	ps.pos_World = mul( vs.pos_Model , worldMat ).xyz;
	
	ps.normVec_World = mul( float4(vs.norm_Model,0.0f) , litMat ).xyz;

	ps.viewVec_World = CameraPos_World - ps.pos_World;
	
	ps.lightVec_World = LightPos_World - ps.pos_World;
	
    
    return ps;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS( PS_INPUT ps ) : SV_Target
{
	
	float3 LightColor = float3(1.0f,1.0f,1.0f);
	float  LightPower = 90.0f;
	
	float distance = length( LightPos_World - ps.pos_World );
	
	float3 LightIntensity = LightColor * ( LightPower / (distance*distance) );
	
	
	
	float3 MaterialDiffuseColor  = txDiffuse.Sample( samLinear, ps.UV ).rgb;
	float3 MaterialAmbientColor  = float3(0.1f,0.1f,0.1f) * MaterialDiffuseColor;
	float3 MaterialSpecularColor = float3(0.3f,0.3f,0.3f);
	
	
	
	// Diffuse angle	
	float3 n = normalize( ps.normVec_World );
	float3 l = normalize( ps.lightVec_World );
	float cosTheta = clamp( dot(n,l), 0.0f, 1.0f );
	
	// Specular angle
	float3 E = normalize( ps.viewVec_World );
	float3 R = reflect(-l,n);
	float cosAlpha = clamp( dot(E,R), 0.0f, 1.0f );
   	
	
	
	float3 mcolor = MaterialAmbientColor + 
					MaterialDiffuseColor  * LightIntensity * cosTheta + 
					MaterialSpecularColor * LightIntensity * pow(cosAlpha,5.0f);
	
	
	return float4( mcolor , 1.0f );
	
}



//// This is Shaders for VTX_PC

struct VS_PC_INPUT
{
    float4 pos_Model : POSITION;
	float4 vertexCol : COLOR;
};

struct PS_PC_INPUT
{
    float4 pos_Clip  	: SV_POSITION;
	float4 fragCol 		: COLOR;
};

PS_PC_INPUT VS_PC( VS_PC_INPUT vs )
{
	PS_PC_INPUT ps = (PS_PC_INPUT)0;
	
	vs.pos_Model.w = 1.0f;
	ps.pos_Clip = mul( vs.pos_Model, MVP[1] );
	
	ps.fragCol = vs.vertexCol;
	
    return ps;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS_PC( PS_PC_INPUT ps ) : SV_Target
{
	
		
	return ps.fragCol;
	
}


















