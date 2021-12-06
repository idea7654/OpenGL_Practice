#pragma warning(disable: 4996)


#define GLFW_INCLUDE_NONE
#include "./GLFW/glfw3.h"

#include "./GLAD/glad.h"


#include "./glm/glm.hpp"
#include "./glm/gtc/matrix_transform.hpp"

#include <iostream>
#include <vector>


#include "Mesh.h"


#define NUM_OBJECT	4


GLuint m_program[NUM_OBJECT];		// should be in Shader class

GLuint m_VAO[NUM_OBJECT];
GLuint VBO[NUM_OBJECT], EBO[NUM_OBJECT];	// at least VBO and EBO should be encapsulated...

GLuint m_Texture[NUM_OBJECT - 1];		// should be in MESH class

int m_NUM_MESH_INDEX[NUM_OBJECT];




// At Initialise
void GL_Setup_Shader_Program( GLuint &program_ID , const char *vs_file , const char *fs_file );
void GL_Setup_V_Buffers_from_Mesh( int vKind , MESH* mesh , GLuint &VAO , GLuint &VBO , GLuint &EBO );
void GL_Setup_Textures( GLuint &tex_ID , const char *filename );

void GL_Change_Aspect_Ratio( int , int );



void Init_GL( GLFWwindow* window )
{

	// glfw windows to GL context
	glfwMakeContextCurrent(window);

	// V-Sync: 0/1 --> framerate goes approx 60/750 Hz
	glfwSwapInterval(1);




	// Init GL
	
	//gladLoadGLES2Loader((GLADloadproc)glfwGetProcAddress); 
	gladLoadGL( );


	// Show Info
	std::cout	<< "[OpenGL ver] \t"	<< glGetString(GL_VERSION)					<< "\n"
				<< "[GLSL ver] \t"		<< glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n"
				<< "[Vendor] \t"		<< glGetString(GL_VENDOR)					<< "\n"
				<< "[Renderer] \t"		<< glGetString(GL_RENDERER)					<< "\n"	  
	<< std::endl;

	

	glEnable(GL_DEPTH_TEST);
	glClearDepthf(1.0f);
	glClearColor(0.2f, 0.2f, 0.5f, 0.0f);



	// MEsh & Shaders
	
	MESH mesh[NUM_OBJECT];


	// Obj #1 (simple octahedron)
	mesh[0].Init( "./cpp/assets/octahedron.obj" );		m_NUM_MESH_INDEX[0] = mesh[0].indices.size();	
	GL_Setup_Shader_Program( m_program[0] , "./cpp/ColorOnly.vs" , "./cpp/ColorOnly.fs" );

	// Set vertex buffers for Obj #1	
	GL_Setup_V_Buffers_from_Mesh( 34 , &mesh[0] , m_VAO[0] , VBO[0] , EBO[0] );



	// Obj #2 (Teapot with Texture)
	/*mesh[1].Init( "./cpp/assets/cube.obj" );		m_NUM_MESH_INDEX[1] = mesh[1].indices.size();
	GL_Setup_Shader_Program( m_program[1] , "./cpp/TexPhong.vs" , "./cpp/TexPhong.fs" );

	GL_Setup_V_Buffers_from_Mesh( 333 , &mesh[1] , m_VAO[1] , VBO[1] , EBO[1] );*/

	
	for (int i = 1; i < 4; i++)
	{
		mesh[i].Init("./cpp/assets/cube.obj");		m_NUM_MESH_INDEX[i] = mesh[i].indices.size();
		GL_Setup_Shader_Program(m_program[i], "./cpp/TexPhong.vs", "./cpp/TexPhong.fs");

		GL_Setup_V_Buffers_from_Mesh(333, &mesh[1], m_VAO[i], VBO[i], EBO[i]);
		switch (i)
		{
		case 1:
			GL_Setup_Textures(m_Texture[0], "./cpp/assets/cube1.jpg");
			break;
		case 2:
			GL_Setup_Textures(m_Texture[1], "./cpp/assets/cube2.jpg");
			break;
		case 3:
			GL_Setup_Textures(m_Texture[2], "./cpp/assets/cube3.jpg");
			break;
		default:
			break;
		}
	}
	

	// Load & Init Texture
	//GL_Setup_Textures( m_Texture , "./cpp/assets/OpenGL_Power.jpg" );
	//GL_Setup_Textures(m_Texture, "./cpp/assets/cube1.jpg");


	// Initialise Projection Matrix & ViewPort
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	GL_Change_Aspect_Ratio( width, height );
	
}




// In Loop
void GL_Control_Shader_for_Object_One( GLuint &m_program );
void GL_Control_Shader_for_Object_Two( GLuint &m_program , GLuint &m_Texture, int Num );


void RenderScene_GL( void )		// without swap
{
	// Clear Color & Depth Buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// DrawCall for Object #1
		glUseProgram(m_program[0]);								// Select Shader Program for Obj #1
		GL_Control_Shader_for_Object_One( m_program[0] );	// Uniforms for Shader #1		

		glBindVertexArray(m_VAO[0]);	// Bind VAO for Obj #1
		glDrawElements(GL_TRIANGLES, (GLsizei)m_NUM_MESH_INDEX[0], GL_UNSIGNED_INT, 0);
		
		

		// DrawCall for Object #2
		glUseProgram(m_program[1]);		
		GL_Control_Shader_for_Object_Two( m_program[1] , m_Texture[0], 1 );
				
		glBindVertexArray(m_VAO[1]);
		glDrawElements(GL_TRIANGLES, (GLsizei)m_NUM_MESH_INDEX[1], GL_UNSIGNED_INT, 0);

		glUseProgram(m_program[2]);
		GL_Control_Shader_for_Object_Two(m_program[2], m_Texture[1], 2);

		glBindVertexArray(m_VAO[2]);
		glDrawElements(GL_TRIANGLES, (GLsizei)m_NUM_MESH_INDEX[2], GL_UNSIGNED_INT, 0);

		glUseProgram(m_program[3]);
		GL_Control_Shader_for_Object_Two(m_program[3], m_Texture[2], 3);

		glBindVertexArray(m_VAO[3]);
		glDrawElements(GL_TRIANGLES, (GLsizei)m_NUM_MESH_INDEX[3], GL_UNSIGNED_INT, 0);


		// UnBind VAOs
		glBindVertexArray(0);



//	Swap is gone out of here
//	glfwSwapBuffers(window);

}


void Clean_GL( void )
{
	for( int i=0 ; i<NUM_OBJECT ; i++ )
	{
		glDeleteProgram( m_program[i] );

		glDeleteVertexArrays(1, &m_VAO[i]);
		glDeleteBuffers(1, &VBO[i]);
		glDeleteBuffers(1, &EBO[i]);
	}

	glDeleteTextures( 1, &m_Texture[0] );
	glDeleteTextures(1, &m_Texture[1]);
	glDeleteTextures(1, &m_Texture[2]);
}






//GLFWwindow* window;

//glm::mat4 ViewMatrix;
//glm::mat4 ProjectionMatrix;

/*
//void Compute_Matrices_From_Inputs()
void Compute_Matrices_From_Inputs( glm::mat4 &ViewMatrix )
{
	// Projection matrix : 45¡Æ Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	ProjectionMatrix = glm::perspective(	glm::radians(initialFoV), 
											4.0f/3.0f, 
											0.1f, 
											100.0f			);


	// Camera matrix
	ViewMatrix = glm::lookAt(	glm::vec3(0.0f, 0.0f, cameraZ),
										glm::vec3(0.0f, 0.0f, 0.0f),
										glm::vec3(0.0f, 1.0f, 0.0f)			);
}
*/

//glm::mat4 getViewMatrix() {		return ViewMatrix;		}

//glm::mat4 getProjectionMatrix(){	return ProjectionMatrix;	}



// Mesh & Vertex Buffers
	// Read our .obj file
/*	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> uvs;
	std::vector<unsigned int> indices;

	bool res = loadOBJ("./cpp/teapot.obj", vertices, uvs, normals, indices);

	NUM_VERTEX = vertices.size();


	// Load it into a VBO
	glGenBuffers(1, &m_vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, NUM_VERTEX * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

	glGenBuffers(1, &m_normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);

	glGenBuffers(1, &m_uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);
*/