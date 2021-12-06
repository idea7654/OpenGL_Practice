//#version 320 es
//precision mediump float;

#version 460 core


layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexNormal_modelspace;
layout(location = 2) in vec2 vertexUV;

out vec2 UV;
out vec3 pos_World;

out vec3 normVec_World;
out vec3 viewVec_World;
out vec3 lightVec_World;


uniform mat4 MVP;
uniform mat4 worldMat;
uniform vec3 LightPos_World;
uniform vec3 CameraPos_World;



void main()
{
	// Basic transformation
	gl_Position = MVP * vec4(vertexPosition_modelspace, 1.0f);		// Position in Clip-Space
	UV = vertexUV;
	
	
	
	// for Lighting etc.	
	pos_World = (worldMat * vec4(vertexPosition_modelspace,1.0f)).xyz;

	vec3 VertexPos = ( worldMat * vec4(vertexPosition_modelspace,1)).xyz;
	
	viewVec_World  = CameraPos_World - VertexPos;
	
	lightVec_World = LightPos_World - VertexPos;
	
	normVec_World  = transpose(inverse(mat3(worldMat))) * vertexNormal_modelspace;
	
	
	
}