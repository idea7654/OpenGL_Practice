#pragma warning(disable: 4996)

#define GLFW_EXPOSE_NATIVE_WIN32
#include "./GLFW/glfw3.h"
#include "./GLFW/glfw3native.h"

void Init_GL(GLFWwindow* window);		void RenderScene_GL(void);		void Clean_GL(void);
bool Init_DX(HWND hwnd);				void RenderScene_DX(void);		void Clean_DX(void);


GLFWwindow* Create_GLFW_Wnd_and_CallBacks(bool);

bool is_dx = false;

int main(void)
{

	GLFWwindow* window = Create_GLFW_Wnd_and_CallBacks(is_dx);	// if DX, window hint is NO_API


	//////////////////////////////////////////////////////////////////

	if (is_dx)
	{
		Init_DX(glfwGetWin32Window(window));
		while (!glfwWindowShouldClose(window))
		{
			glfwWaitEventsTimeout(0.01);

			RenderScene_DX();			// swap not included in GL

			glfwSwapBuffers(window);		// Swap
			glfwPollEvents();
		}

		Clean_DX();
	}
	else
	{
		Init_GL(window);

		while (!glfwWindowShouldClose(window))
		{
			glfwWaitEventsTimeout(0.01);

			RenderScene_GL();			// swap not included in GL

			glfwSwapBuffers(window);		// Swap
			glfwPollEvents();
		}


		Clean_GL();
	}


	glfwDestroyWindow(window);

	glfwTerminate();
	return 0;


}



