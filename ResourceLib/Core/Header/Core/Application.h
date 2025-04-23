#pragma once

///ModernOpenGl Include
#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "pch.h"

///Standar include
#include <iostream>
#include <thread>

//Project Include
#include "Core/camera.h"

namespace Rendering
{
	class Application
	{
	public:
		Application(void);

		Application(int width, int height);

		void createWindow();

		static void framebufferSizeCallback(GLFWwindow* window, int width, int height);

		static void mouseCallback(GLFWwindow* window, double xpos, double ypos);
		
		float getDeltaTime();

		void updateDeltaTime();

		~Application(void);

		GLFWwindow* m_screen;
		int m_width;
		int m_height;
		Visual::Camera* m_camera;
		float			m_lastFrame = 0;
		float			m_deltaTime = 0;

		static Application* instance;
	};
}