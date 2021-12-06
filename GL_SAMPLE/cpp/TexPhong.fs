//#version 320 es
//precision mediump float;

#version 460 core


in vec2 UV;
in vec3 pos_World;		// for calculating distance
in vec3 normVec_World;
in vec3 viewVec_World;
in vec3 lightVec_World;		// interpolated light direction for fragments

out vec4 fragColor;

uniform sampler2D TextureMap;
uniform vec3 LightPos_World;

void main()
{
	vec3 LightColor = vec3(1.0f,1.0f,1.0f);
	float LightPower = 90.0f;
	
	vec3 MaterialDiffuseColor = texture( TextureMap, UV ).rgb;
	vec3 MaterialAmbientColor = vec3(0.1f,0.1f,0.1f) * MaterialDiffuseColor;
	vec3 MaterialSpecularColor = vec3(0.3f,0.3f,0.3f);
	
	
	
	float distance = length( LightPos_World - pos_World );
	vec3 n = normalize( normVec_World );
	vec3 l = normalize( lightVec_World );
	float cosTheta = clamp( dot( n,l ), 0.0f , 1.0f );
	
	vec3 E = normalize(viewVec_World);
	vec3 R = reflect(-l,n);
	float cosAlpha = clamp( dot( E,R ), 0.0f , 1.0f );
   	
	
	
	vec3 mcolor = 	MaterialAmbientColor + 
					MaterialDiffuseColor  * LightColor * LightPower * cosTheta / (distance*distance) + MaterialSpecularColor * LightColor * LightPower * pow(cosAlpha,5.0f) / (distance*distance);
	
	fragColor = vec4( mcolor , 1.0f );
}  