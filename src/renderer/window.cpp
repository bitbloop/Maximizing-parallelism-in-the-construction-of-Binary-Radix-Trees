#include "renderer.h"
#include <iostream>


namespace
{
	GLFWwindow* _window{ nullptr };
	int _window_width{ 1700 };
	int _window_height{ 880 };
};


namespace
{
	/**	*/
	bool create_context() 
	{
		if (glfwInit()) return true;
		std::cout << "ERROR: could not start GLFW3" << std::endl;
			return false;
	}

	/**	*/
	bool create_window()
	{
		_window = glfwCreateWindow(_window_width, _window_height, "Hello Triangle", NULL, NULL);
		if (!_window) {
			std::cout << "ERROR: could not open window with GLFW3" << std::endl;
			glfwTerminate();
			return false;
		}
		glfwMakeContextCurrent(_window);
		return true;
	}

	/** */
	bool create_extension_bindings()
	{
		//glewExperimental = GL_TRUE;
		glewInit();
		return true;
	}

	/** */
	void debug_print()
	{
		// get version info
		const GLubyte* renderer{ glGetString(GL_RENDERER) }; // get renderer string
		const GLubyte* version{ glGetString(GL_VERSION) }; // version as a string
		std::cout << "Renderer: " << renderer;
		std::cout << "OpenGL version supported " << version;
		std::cout << std::endl;
	}

	/** */
	void init_states()
	{
		// TODO: THIS WILL BE MOVED ELSEWHERE. IT IS PROJECT SPECIFIC

		// tell GL to only draw onto a pixel if the shape is closer to the viewer
		glEnable(GL_DEPTH_TEST); // enable depth-testing
		//glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
		glEnable(GL_TEXTURE_2D);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_BLEND);
		//glDisable(GL_CULL_FACE);
		//glEnable(GL_BLEND); 
		//glBlendFunc(GL_SRC_ALPHA, GL_SRC_COLOR);
	}
}

/** */
bool renderer::window::create()
{
	if (!create_context()) return false;
	if (!create_window()) return false;
	if (!create_extension_bindings()) return false;

	init_states();
	debug_print();

	space::update_viewport_matrix(_window_width, _window_height);
	space::update_view_matrix({ 0,0,3 }, { 0,0,0 });
	space::update_projection_matrix(_window_width, _window_height);

	return true;
}

/** */
void renderer::window::kill()
{
	glfwTerminate();
}

/** */
void renderer::window::pool_events_and_swap_buffers()
{
	// update other events like input handling 
	glfwPollEvents();
	// put the stuff we've been drawing onto the display
	glfwSwapBuffers(_window);
}

/** */
bool renderer::window::should_window_close()
{
	return glfwWindowShouldClose(_window);
}



