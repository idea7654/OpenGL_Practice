#pragma warning(disable: 4996)
#include <stdio.h>
#include <stdlib.h>		// for exit()

#include "./GLFW/glfw3.h"

#include "./glm/glm.hpp"
#include "./glm/gtc/matrix_transform.hpp"





void error_callback(int error, const char* description)
{
	printf("Error: %s\n", description);
}









glm::vec2 drag_start(0.0f);		// cursor position when button pressed

glm::mat4 model_rotation(1.0f);	// rotation matrix from prev dragging
glm::mat4 model_rotation2(1.0f);
glm::mat4 model_rotation3(1.0f);
glm::mat4 joint1Rotation(1.0f);	// rotation by current dragging epoch
glm::mat4 joint2Rotation(1.0f);
glm::mat4 joint3Rotation(1.0f);

glm::mat4 model_translation(1.0f);
glm::mat4 drag_translation(1.0f);

glm::mat4 Get_Drag_Rotation(int Num) { if (Num == 1) return joint1Rotation; if (Num == 2) return joint2Rotation; if (Num == 3) return joint3Rotation; }


static bool is_drag = false;
static bool is_left_mouse = true;

static float RotateAngle1 = 0.0f;
static float RotateAngle2 = 0.0f;
static float RotateAngle3 = 0.0f;
static int SelectedJoint = 1;
glm::vec4 joint2Vec = glm::vec4(glm::vec3(-0.5f, 0.0f, 0.0f), 1.0f);
glm::vec4 joint3Vec = glm::vec4(glm::vec3(0, 0, 1.5f), 1.0f);

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)		glfwSetWindowShouldClose(window, GLFW_TRUE);

	if (key == GLFW_KEY_1 && action == GLFW_PRESS) //Select Joint
		SelectedJoint = 1;
	if (key == GLFW_KEY_2 && action == GLFW_PRESS)
		SelectedJoint = 2;
	if (key == GLFW_KEY_3 && action == GLFW_PRESS)
		SelectedJoint = 3;

	glm::mat4 view_matrix(1.0f);
	//glm::mat3 to_world = glm::inverse(glm::mat3(view_matrix));
	if (key == GLFW_KEY_UP && action == GLFW_PRESS)
	{
		if (SelectedJoint == 1)
		{
			RotateAngle1 += 0.1f;
		}
		if (SelectedJoint == 2)
		{
			RotateAngle2 += 0.1f;
		}
		if (SelectedJoint == 3)
		{
			RotateAngle3 += 0.1f;
		}
		//glm::mat4 RotateMatParentParent = glm::rotate(glm::mat4(1.0f), RotateAngle1, glm::vec3(1.0f, 0.0f, 0.0f));
		//glm::mat4 RotateMatParent = glm::translate(RotateMatParentParent, glm::vec3(-0.5f, 0.0f, 0.0f)) * glm::rotate(RotateMatParentParent, RotateAngle2, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::translate(RotateMatParentParent, glm::vec3(0.5f, 0.0f, 0.0f));
		//glm::mat4 RotateMat = glm::translate(RotateMatParentParent, glm::vec3(1.5f, 0.0f, 0.0f)) * glm::rotate(RotateMatParent, RotateAngle3, glm::vec3(0.0f, 0.0f, 1.0f)) * glm::translate(RotateMatParentParent, glm::vec3(-1.5f, 0.0f, 0.0f));

		glm::mat4 RotateMatParentParent = glm::rotate(glm::mat4(1.0f), RotateAngle1, glm::vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 RotateMatParent = RotateMatParentParent * glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, 0.f, 0.f)) * glm::rotate(glm::mat4(1.0f), RotateAngle2, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, 0.f, 0.f));
		glm::mat4 RotateMat = RotateMatParent * glm::translate(glm::mat4(1.0f), glm::vec3(1.5f, 0.0f, 0.0f)) * glm::rotate(glm::mat4(1.0f), RotateAngle3, glm::vec3(0.0f, 0.0f, 1.0f)) * glm::translate(glm::mat4(1.0f), glm::vec3(-1.5f, 0.0f, 0.0f));
		
		joint1Rotation = RotateMatParentParent * model_rotation;
		joint2Rotation = RotateMatParent * model_rotation2;
		joint3Rotation = RotateMat * model_rotation3;
	}
	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
	{
		if (SelectedJoint == 1)
		{
			RotateAngle1 -= 0.1f;
		}
		if (SelectedJoint == 2)
		{
			RotateAngle2 -= 0.1f;
		}
		if (SelectedJoint == 3)
		{
			RotateAngle3 -= 0.1f;
		}
		glm::mat4 RotateMatParentParent = glm::rotate(glm::mat4(1.0f), RotateAngle1, glm::vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 RotateMatParent = RotateMatParentParent * glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, 0.f, 0.f)) * glm::rotate(glm::mat4(1.0f), RotateAngle2, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, 0.f, 0.f));
		glm::mat4 RotateMat = RotateMatParent * glm::translate(glm::mat4(1.0f), glm::vec3(1.5f, 0.0f, 0.0f)) * glm::rotate(glm::mat4(1.0f), RotateAngle3, glm::vec3(0.0f, 0.0f, 1.0f)) * glm::translate(glm::mat4(1.0f), glm::vec3(-1.5f, 0.0f, 0.0f));

		joint1Rotation = RotateMatParentParent * model_rotation;
		joint2Rotation = RotateMatParent * model_rotation2;
		joint3Rotation = RotateMat * model_rotation3;
	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{

	if ( (button==GLFW_MOUSE_BUTTON_LEFT)  ||  (button==GLFW_MOUSE_BUTTON_RIGHT)  )

	if ( button <= GLFW_MOUSE_BUTTON_RIGHT )	// left ot right button
	{
		switch( action )
		{
			case GLFW_PRESS:
	
				is_drag = true;
				double xpos, ypos;
				glfwGetCursorPos(window, &xpos, &ypos);
				drag_start = glm::vec2(xpos, ypos);

				if( button == GLFW_MOUSE_BUTTON_LEFT )	is_left_mouse = true;
				else									is_left_mouse = false;
	
			break;


			case GLFW_RELEASE:			

				is_drag = false;
		
				if( button == GLFW_MOUSE_BUTTON_LEFT )
				{
					model_rotation = joint1Rotation;
					model_rotation2 = joint2Rotation;
					model_rotation3 = joint3Rotation;
				}
				else
				{
					model_translation = drag_translation;
				}
	
			break;
		}

	}


}

// mouse wheel
static float cameraZ = 30.0f;
static float cameraX = 0.0f;
static float cameraY = 0.0f;

static float camPos[3] = { 0 };

static bool IS_DX = false;

// when cursor is moving (regardless button pressed)
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	if ( !is_drag )	return;

	// drag in screen space
	glm::vec2 drag_vec = glm::vec2(xpos - drag_start.x, drag_start.y - ypos);	
	float dragLen = glm::length(drag_vec);


	glm::mat4 view_matrix(1.0f);
	glm::mat3 to_world = glm::inverse(glm::mat3(view_matrix));	// This has to be done if camera moves		

	if( is_left_mouse )
	{
		glm::vec3 rot_axis = glm::normalize( to_world * glm::vec3( -drag_vec.y, drag_vec.x, 0.0f )  );

		float angle = dragLen / 800.0f / 2.0f * 3.14159265358979323846f; //800->height

		glm::mat4 RotateMatParentParent = glm::rotate(glm::mat4(1.0f), RotateAngle1, glm::vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 RotateMatParent = RotateMatParentParent * glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, 0.f, 0.f)) * glm::rotate(glm::mat4(1.0f), RotateAngle2, glm::vec3(0.0f, 1.0f, 0.0f)) * glm::translate(glm::mat4(1.0f), glm::vec3(0.5f, 0.f, 0.f));
		glm::mat4 RotateMat = RotateMatParent * glm::translate(glm::mat4(1.0f), glm::vec3(1.5f, 0.0f, 0.0f)) * glm::rotate(glm::mat4(1.0f), RotateAngle3, glm::vec3(0.0f, 0.0f, 1.0f)) * glm::translate(glm::mat4(1.0f), glm::vec3(-1.5f, 0.0f, 0.0f));
	
		joint1Rotation = RotateMatParentParent * glm::rotate(glm::mat4(1.0f), angle, rot_axis) * model_rotation;
		joint2Rotation = RotateMatParent * glm::rotate(glm::mat4(1.0f), angle, rot_axis) * model_rotation2;
		joint3Rotation = RotateMat * glm::rotate(glm::mat4(1.0f), angle, rot_axis) * model_rotation3;
	}
	else	// drag with right button
	{
		//float dist = dragLen * 0.01f;
		//glm::vec2 transVec = glm::normalize(drag_vec) * dist;

		//drag_translation = glm::translate( glm::mat4(1.0f),  glm::vec3(transVec,0.0f) ) * model_translation;
	}


//	drag_rotation = drag_translation * drag_rotation;

}

// For DX
#include "./glm/gtc/type_ptr.hpp"
void Fill_Drag_Rotation( float f[16] )
{
//	drag_rotation = glm::translate( glm::mat4(1.0f) , glm::vec3(2.0f,3.0f,4.0f) );

	const float *pSource = (const float*)glm::value_ptr(joint1Rotation);
	for (int i = 0; i < 16; ++i )
		f[i] = pSource[i];
}




float* Get_Camera_Pos( void )
{
	//camPos[0] = camPos[1] = 0.0f;
	camPos[0] = cameraX;
	camPos[1] = cameraY;
	camPos[2] = cameraZ;

	return camPos;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	cameraZ -= (float)yoffset*2.0f;
}




void GL_Change_Aspect_Ratio( int , int );
void DX_Change_Aspect_Ratio( int , int );

void GL_framebuffer_size_callback( GLFWwindow* window, int width, int height )
{
	if( IS_DX )	DX_Change_Aspect_Ratio( width , height );
	else		
		GL_Change_Aspect_Ratio( width , height );
}





// Create a window
GLFWwindow* Create_GLFW_Wnd_and_CallBacks( bool is_dx )
{
	IS_DX = is_dx;

	GLFWwindow* win;

	if( !glfwInit() )	exit(EXIT_FAILURE);
  
	//glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
 //   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
 //   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);
 //

	//// Forget hint !!
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	if( IS_DX )	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);


    win = glfwCreateWindow(640, 480, "Simple GL-DX example", NULL, NULL);

    if (!win)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }


	// All CallBack Functions
	glfwSetErrorCallback			( error_callback );
	
	glfwSetKeyCallback				( win , key_callback );

	glfwSetMouseButtonCallback		( win , mouse_button_callback );
	glfwSetCursorPosCallback		( win , cursor_position_callback );
	glfwSetScrollCallback			( win , scroll_callback	);

	
	glfwSetFramebufferSizeCallback( win , GL_framebuffer_size_callback );
	

	return win;
}




#include <iostream>
#include <fstream>
#include <string>

std::string Read_File_String(const char *filePath)
{
	
	std::ifstream fileStream(filePath, std::ios::in);	// constructor: mode: in --> read mode

	if (!fileStream.is_open()) 
	{
		std::cerr << "Could not read file " << filePath << ". File does not exist." << std::endl;
		return "";
	}

	
	std::string code;

	std::string line = "";
	while (!fileStream.eof()) {
		std::getline(fileStream, line);
		code.append(line + "\n");
	}

	fileStream.close();

	return code;
}




//#include <time.h>

float Get_Sim_Time( void )
{
	static float tick = 0.0f;

//	if( (unsigned int)clock() % 10 == 0 )
//	{
		tick += 0.00628f * 2.0f;

		if( tick > 6.28f )	tick -= 6.28f;
//	}

	return tick;


	/*	clock_t start, finish;
	double duration;

	start = clock(); // 시작지점 시간


	finish = clock(); // 종료지점 시간
	duration = (double)(finish-start)/CLOCKS_PER_SEC; // (종료지점 시간 - 시작지점 시간)/1000
*/


}

