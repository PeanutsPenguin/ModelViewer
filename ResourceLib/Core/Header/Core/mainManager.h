#pragma once 

#include "Core/Application.h"
//#include "Core/taskManager.h"
#include "Resources/ResourceManager.h"
#include "Resources/Scene.h"

//Model
#define SKULL "Assets/Meshes/skull.obj"
#define LOCOMOTIVE "Assets/Meshes/locomotive.obj"
#define BUNNY "Assets/Meshes/bunny.obj"
#define CUBE "Assets/Meshes/cube/cube_with_texture.obj"
#define PADORU "Assets/Meshes/padoru.obj"

//Texture
#define CUBETEXTURE "Assets/Meshes/cube/cube_with_texture.png"
#define SKULLTEXTURE "Assets/Meshes/skull.jpg"
#define PADORUTEXTURE "Assets/Meshes/padoru.png"

#define OFFSET 5
#define COOLDOWN 0.2f
#define SHIFT_MULTIPLIER 5

namespace Core
{
	class MainManager
	{
		public:
		MainManager();

		MainManager(int x, int y);

		void render();

		void processInput();

		void movementInput(GLFWwindow* window, Visual::Camera& cam, float speed);

		void processLoadInput(GLFWwindow* window);
		
		void loadAndAdd(const char* filename);

		void resetAll();

		void closeApp();

		~MainManager();

		bool	m_monothread = true;
		float	m_inputCoolDown = 0;
		bool	m_fill		  = false;

		private:
		Rendering::Application*		 m_app;
		Resources::ResourcesManager* m_manager;
		Resources::Shader*			 m_shaders;
		Resources::Scene*			 m_scene;
	};
}



