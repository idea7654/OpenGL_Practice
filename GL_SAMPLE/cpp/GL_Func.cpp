#pragma warning(disable: 4996)


#include "./GLAD/glad.h"


#include "./glm/glm.hpp"
#include "./glm/gtc/matrix_transform.hpp"

#include <iostream>
#include <vector>





//////////// Shader /////////////////////////////////////////////////////////////////////////////

void Shader_Compile_Error( GLuint vs_ID , GLuint fs_ID , GLuint &program_ID )
{
	GLint Result = GL_FALSE;
	int InfoLogLength;


	printf( "Vertex Shader Compile Status\t---> " );

	glGetShaderiv(vs_ID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(vs_ID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 )
	{
		printf( "Error!\n" );
		std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(vs_ID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}
	else
	{
		printf( "Done!\n" );
	}


	
	printf( "Fragment Shader Compile Status  ---> " );

	glGetShaderiv(fs_ID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(fs_ID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 )
	{
		printf( "Error!\n" );
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(fs_ID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}
	else
	{
		printf( "Done!\n" );
	}


	
	printf( "Program Link Status\t\t---> " );
	
	glGetProgramiv(program_ID, GL_LINK_STATUS, &Result);
	glGetProgramiv(program_ID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 )
	{
		printf( "Error!\n" );
		std::vector<char> ProgramErrorMessage(InfoLogLength+1);
		glGetProgramInfoLog(program_ID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}
	else
	{
		printf( "Done!\n\n" );
	}

}


std::string Read_File_String( const char *filePath );

void GL_Setup_Shader_Program( GLuint &program_ID , const char *vs_file , const char *fs_file )
{
	// Load Shader Files as strings
	std::string vs_string = Read_File_String( vs_file );
	std::string fs_string = Read_File_String( fs_file );
	const char *vs_src = vs_string.c_str();
	const char *fs_src = fs_string.c_str();


	// GL Shader & Program
	GLuint vs_ID = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs_ID, 1, &vs_src, NULL);
	glCompileShader(vs_ID);

	GLuint fs_ID = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs_ID, 1, &fs_src, NULL);
	glCompileShader(fs_ID);

	program_ID = glCreateProgram();
	glAttachShader(program_ID, vs_ID);
	glAttachShader(program_ID, fs_ID);
	glLinkProgram(program_ID);
	
	// Error Check
	Shader_Compile_Error( vs_ID , fs_ID , program_ID );

	// After successfully attached to the program then delete shader objects
	glDeleteShader(vs_ID);
	glDeleteShader(fs_ID);  
}






//////////// V Buffers /////////////////////////////////////////////////////////////////////////////

// Different Vertex Layouts
struct VTX_PNT	{	float pos[3];	float norm[3];	float tex[2];	}; //Position Normal Texture //Stride - 12bytes
struct VTX_PC	{	float pos[3];	float col[4];	}; //Position Color //Stride - 8bytes



#include "Mesh.h"

void GL_Setup_V_Buffers_from_Mesh( int vKind , MESH* mesh , GLuint &VAO , GLuint &VBO , GLuint &EBO )
{
	glGenVertexArrays(1, &VAO); //Vertex Array Object
	glGenBuffers(1, &VBO); //Vertex Buffer Object
	glGenBuffers(1, &EBO); //Element Buffer Object(Index Buffer)
  
    glBindVertexArray(VAO);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->indices.size() * sizeof(unsigned int), &mesh->indices[0], GL_STATIC_DRAW);
    
		
		int NUM_VTX = mesh->vertices.size();

		if( vKind == 333 )	// Vertex layout Position-Normal-TexCoord
		{
			
			VTX_PNT* vtx_pnt = new VTX_PNT[NUM_VTX];

			for( int n=0 ; n<NUM_VTX ; n++ )
			{
				vtx_pnt[n].pos[0] = mesh->vertices[n].Position.x;
				vtx_pnt[n].pos[1] = mesh->vertices[n].Position.y;
				vtx_pnt[n].pos[2] = mesh->vertices[n].Position.z;

				vtx_pnt[n].norm[0] = mesh->vertices[n].Normal.x;
				vtx_pnt[n].norm[1] = mesh->vertices[n].Normal.y;
				vtx_pnt[n].norm[2] = mesh->vertices[n].Normal.z;

				vtx_pnt[n].tex[0] = mesh->vertices[n].TexCoord.x;
				vtx_pnt[n].tex[1] = mesh->vertices[n].TexCoord.y;
			}			

			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, NUM_VTX * sizeof(VTX_PNT), &vtx_pnt[0], GL_STATIC_DRAW);

			delete [] vtx_pnt;


			// vertex positions
			glEnableVertexAttribArray(0);	
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VTX_PNT), (void*)0 );
		
			// vertex normals
			glEnableVertexAttribArray(1);	
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VTX_PNT), (void*)offsetof(VTX_PNT,norm) );
		
			// vertex texture coords
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VTX_PNT), (void*)offsetof(VTX_PNT,tex) );
		}


		if( vKind == 34 )	// Vertex layout Position-Color
		{
			
			VTX_PC* vtx_pc = new VTX_PC[NUM_VTX];

			for( int n=0 ; n<NUM_VTX ; n++ )
			{
				vtx_pc[n].pos[0] = mesh->vertices[n].Position.x;
				vtx_pc[n].pos[1] = mesh->vertices[n].Position.y;
				vtx_pc[n].pos[2] = mesh->vertices[n].Position.z;

				vtx_pc[n].col[0] = mesh->vertices[n].Color.r;
				vtx_pc[n].col[1] = mesh->vertices[n].Color.g;
				vtx_pc[n].col[2] = mesh->vertices[n].Color.b;
				vtx_pc[n].col[3] = mesh->vertices[n].Color.a;
			}

			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, NUM_VTX * sizeof(VTX_PC), &vtx_pc[0], GL_STATIC_DRAW);

			delete [] vtx_pc;

			// vertex positions
			glEnableVertexAttribArray(0);	
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VTX_PC), (void*)0 );			

			// vertex color
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(VTX_PC), (void*)offsetof(VTX_PC,col) );
		}


    glBindVertexArray(0);	// Unbind VAO


}




//////////// Textures /////////////////////////////////////////////////////////////////////////////

// for texture
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void GL_Setup_Textures( GLuint &tex_ID , const char *filename )
{
	int width, height, nrChannels;
	unsigned char *data = stbi_load( filename , &width, &height, &nrChannels, 0);

	glGenTextures(1, &tex_ID);
	glBindTexture(GL_TEXTURE_2D, tex_ID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);	

}





/////////// IN LOOP Shader Control ///////////////////////////////////////////////////////////////////////////////

glm::mat4 ProjectionMatrix;

void GL_Change_Aspect_Ratio( int width , int height )	// Set ProjMat & Viewport from window width & height
{
	float FoV = glm::radians(45.0f);
	float near = 0.1f;
	float far = 500.0f;

	float AR = (float)width / (float)height;

	ProjectionMatrix  = glm::perspective( FoV, AR, near, far );

	glViewport(0, 0, width, height);

	printf( "Frame Buffer Size %dx%d\n" , width , height );
}

// Common for all Shaders !!
float* Get_Camera_Pos( void );
void GL_SetShader_MVP_Matrices( GLuint &m_program , glm::mat4 WorldMatrix )
{
	float* cam = Get_Camera_Pos();
	glm::mat4 ViewMatrix = glm::lookAt(	glm::vec3( cam[0], cam[1], cam[2] ),	// cam pos
										glm::vec3( 0.0f, 0.0f, 0.0f	),				// At
										glm::vec3( 0.0f, 1.0f, 0.0f	)	);			// Up
	
	glm::mat4 MVP = ProjectionMatrix * ViewMatrix * WorldMatrix;

	
	// Send our transformation to the currently bound shader
	glUniformMatrix4fv( glGetUniformLocation( m_program , "MVP" )	, 1 , GL_FALSE , &MVP[0][0]	);

}



// for lightPos
glm::vec4 lightPos;

// For Object #1
float Get_Sim_Time( void );

void GL_Control_Shader_for_Object_One( GLuint &m_program )
{
	glm::mat4 scaleMat = glm::mat4(1.0f);
	scaleMat = glm::scale( scaleMat , glm::vec3( 0.1f, 0.2f, 0.1f ) );

	glm::mat4 rotMat = glm::mat4(1.0f);
	rotMat = glm::rotate( rotMat , Get_Sim_Time() , glm::vec3(0.0f,1.0f,0.0f) );

	glm::mat4 transMat = glm::mat4(1.0f);
	transMat = glm::translate( transMat , glm::vec3( 15.0f, 7.0f, 0.0f )   );



	glm::mat4 WorldMatrix = rotMat * transMat * scaleMat;

	GL_SetShader_MVP_Matrices( m_program , WorldMatrix );

	
	// for moving light pos
	lightPos = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	lightPos =  WorldMatrix * lightPos;
}



// For Object #2
glm::mat4 Get_Drag_Rotation( void );
//Update
void GL_Control_Shader_for_Object_Two( GLuint &m_program , GLuint &m_Texture, int Num )
{
	// ArcBall Rotation
	glm::mat4 WorldMatrix = Get_Drag_Rotation();

	if (Num == 1)
	{
		//glm::mat4 TranslateMatrix = glm::scale(WorldMatrix, glm::vec3(2.0f, 2.0f, 2.0f));
		glm::mat4 RotationMatrix = glm::rotate(WorldMatrix, 1.570795f, glm::vec3(0.0f, 0.0f, 1.0f));
		glm::mat4 TranslateMatrix = glm::translate(RotationMatrix, glm::vec3(-1.5f, 0.0f, 0.0f));
		glm::mat4 ScaleMatrix = glm::scale(TranslateMatrix, glm::vec3(2.0f, 1.0f, 1.0f));
		GL_SetShader_MVP_Matrices(m_program, ScaleMatrix);
		glUniformMatrix4fv(glGetUniformLocation(m_program, "worldMat"), 1, GL_FALSE, &ScaleMatrix[0][0]);
	}
	if (Num == 2)
	{
		glm::mat4 TranslateMatrix = glm::translate(WorldMatrix, glm::vec3(0.5f, 0.0f, 0.0f));
		glm::mat4 ScaleMatrix = glm::scale(TranslateMatrix, glm::vec3(2.0f, 1.0f, 1.0f));
		GL_SetShader_MVP_Matrices(m_program, ScaleMatrix);
		glUniformMatrix4fv(glGetUniformLocation(m_program, "worldMat"), 1, GL_FALSE, &ScaleMatrix[0][0]);
	}
	if (Num == 3)
	{
		glm::mat4 RotationMatrix = glm::rotate(WorldMatrix, 1.570795f, glm::vec3(0.0f, 0.0f, 1.0f));
		glm::mat4 TranslateMatrix = glm::translate(RotationMatrix, glm::vec3(1.5f, -1.0f, 0.0f));
		glm::mat4 ScaleMatrix = glm::scale(TranslateMatrix, glm::vec3(2.0f, 1.0f, 1.0f));
		GL_SetShader_MVP_Matrices(m_program, ScaleMatrix);
		glUniformMatrix4fv(glGetUniformLocation(m_program, "worldMat"), 1, GL_FALSE, &ScaleMatrix[0][0]);
	}
		
	//GL_SetShader_MVP_Matrices( m_program , WorldMatrix );

	// Send worldMat for Phong Shading
	//glUniformMatrix4fv( glGetUniformLocation( m_program , "worldMat" )	, 1 , GL_FALSE , &WorldMatrix[0][0]	);


	// This onject has lighting & texture //////////////////////
	
	// if fixed light pos
//	lightPos = glm::vec4(-9.0f, 5.0f, 15.0f, 1.0f);

	glUniform3f( glGetUniformLocation(m_program, "LightPos_World") , lightPos.x , lightPos.y , lightPos.z );

	// camera pos
	float* cam = Get_Camera_Pos();
	glUniform3f( glGetUniformLocation(m_program, "CameraPos_World") , cam[0] , cam[1] , cam[2] );

		
	// This may go at initialise if does not change
	// Bind our texture in Texture Unit 0
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_Texture);

	// Set our "myTextureSampler" sampler to use Texture Unit 0
	glUniform1i( glGetUniformLocation(m_program, "TextureMap") , 0 );

}

