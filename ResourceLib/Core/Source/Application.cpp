#include "Core/Application.h"

namespace Rendering
{
    Application* Application::instance = nullptr;

    Application::Application(void)
    {
		this->createWindow();

        GLFWwindow* window = glfwCreateWindow(800, 600, "Hello World", NULL, NULL);
        if (!window) glfwTerminate();

        glfwMakeContextCurrent(window);
        glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
            std::cout << "Failed to initialize GLAD" << std::endl;

        this->m_screen = window;
        this->m_width = 800;
        this->m_height = 600;
        
		this->m_camera = new Visual::Camera();
        Application::instance = this;

		glfwSetInputMode(this->m_screen, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		glfwSetCursorPosCallback(this->m_screen, this->mouseCallback);
    }

    Application::Application(int width, int height)
    {
		this->m_width  = width;
		this->m_height = height;

        this->createWindow();
        GLFWwindow* window = glfwCreateWindow(this->m_width, this->m_height, "Hello World", NULL, NULL);
        if (!window) glfwTerminate();

        glfwMakeContextCurrent(window);
        glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
            std::cout << "Failed to initialize GLAD" << std::endl;

        this->m_screen = window;

		this->m_camera	  = new Visual::Camera();
        Application::instance = this;

		glfwSetInputMode(this->m_screen, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		glfwSetCursorPosCallback(this->m_screen, this->mouseCallback);
	}

	void Application::createWindow() 
    {
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    }

    void Application::framebufferSizeCallback(GLFWwindow* window, int width, int height) 
    { glViewport(0, 0, width, height); }

    void Application::mouseCallback(GLFWwindow* window, double xpos, double ypos)
    {
        Application::instance->m_camera->cameraInputHandler(xpos, ypos);
    }

    float Application::getDeltaTime()
    {
        return this->m_deltaTime;
    }

    void Application::updateDeltaTime()
    {
        ///Calculate the Delta Time
        float currentFrame = glfwGetTime();
        this->m_deltaTime = currentFrame - m_lastFrame;
        this->m_lastFrame = currentFrame;
    }

	Application::~Application(void) 
    { 
        delete this->m_camera;
		glfwTerminate();
		glfwDestroyWindow(this->m_screen);
    }
}

