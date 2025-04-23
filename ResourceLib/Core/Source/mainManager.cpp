#include "Core/mainManager.h"

namespace Core
{
	MainManager::MainManager()
	{
		this->m_app		= new Rendering::Application(1920, 1080);
		this->m_manager = new Resources::ResourcesManager();
		this->m_scene	= new Resources::Scene();
		this->m_shaders = new Resources::Shader();

		unsigned int vertexShader	= this->m_shaders->setShader("Shaders/vertexShaderSrc.glsl", GL_VERTEX_SHADER);
		unsigned int fragmentShader = this->m_shaders->setShader("Shaders/fragmentShaderSrc.glsl", GL_FRAGMENT_SHADER);
		this->m_shaders->m_shaderProgram = this->m_shaders->link({vertexShader, fragmentShader});

		glEnable(GL_DEPTH_TEST);
	}

	MainManager::MainManager(int x, int y)
	{
		this->m_app		= new Rendering::Application(x, y);
		this->m_manager = new Resources::ResourcesManager();
		this->m_scene	= new Resources::Scene();
		this->m_shaders = new Resources::Shader();

		unsigned int vertexShader	= this->m_shaders->setShader("Shaders/vertexShaderSrc.glsl", GL_VERTEX_SHADER);
		unsigned int fragmentShader = this->m_shaders->setShader("Shaders/fragmentShaderSrc.glsl", GL_FRAGMENT_SHADER);
		this->m_shaders->m_shaderProgram = this->m_shaders->link({vertexShader, fragmentShader});

		glEnable(GL_DEPTH_TEST);
	}

	void MainManager::render()
	{
		while (!glfwWindowShouldClose(this->m_app->m_screen))
		{
			/// Clear the m_screen
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			if (this->m_monothread) glClearColor(1, 0, 0, 1);
			else glClearColor(0, 0, 1, 1);

			/// Use our shader Program
			glUseProgram(this->m_shaders->m_shaderProgram);

			/// Update the camera movement
			this->m_app->m_camera->cameraUpdate();

			/// Check the Keyboard Input
			this->processInput();

			/// Calculate the Projection View Model Matrix
			LibMath::Mat4 pvm =
				this->m_app->m_camera->m_projection * this->m_app->m_camera->m_view * this->m_app->m_camera->m_transformation;

			/// Set the draw mode
			if (this->m_fill) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			else glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

			/// Redner every 3D object
			this->m_scene->renderScene(*this->m_shaders, pvm);

			/// Swap Buffers
			glfwSwapBuffers(this->m_app->m_screen);

			/// Process Events
			glfwPollEvents();
		}
	}

	void MainManager::processInput()
	{
		this->m_app->updateDeltaTime();

		/// Calcultate the speed depends on the Delta time
		float speed = this->m_app->m_camera->m_movementSpeed * this->m_app->getDeltaTime();

		if (this->m_inputCoolDown < COOLDOWN + 1) this->m_inputCoolDown += this->m_app->getDeltaTime();

		/// Quit the window when you press ESCAPE
		if (glfwGetKey(this->m_app->m_screen, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(this->m_app->m_screen, true);

		this->movementInput(this->m_app->m_screen, *this->m_app->m_camera, speed);

		if (glfwGetKey(this->m_app->m_screen, GLFW_KEY_0) == GLFW_PRESS && this->m_inputCoolDown > COOLDOWN)
		{
			if(this->m_monothread)
			{
				std::cout << "Switch to Multithread" << '\n';
				this->m_monothread = false;
			}
			else
			{
				std::cout << "Switch to Monothread" << '\n';
				this->m_monothread = true;
			}

			this->m_inputCoolDown = 0;
		}

		if (glfwGetKey(this->m_app->m_screen, GLFW_KEY_DELETE) == GLFW_PRESS && this->m_inputCoolDown > COOLDOWN)
		{
			std::cout << "Reset Everything" << '\n';
			this->resetAll();
		}
			
		
		if (glfwGetKey(this->m_app->m_screen, GLFW_KEY_F) == GLFW_PRESS && this->m_inputCoolDown > COOLDOWN)
		{
			this->m_fill = !this->m_fill;
			this->m_inputCoolDown = 0;
		}

		this->processLoadInput(this->m_app->m_screen);
	}

	void MainManager::movementInput(GLFWwindow* window, Visual::Camera& cam, float speed)
	{
		/// Input WASD to go in every direction
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) speed *= SHIFT_MULTIPLIER;
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) cam.m_position -= cam.m_front * speed;
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) cam.m_position += cam.m_front * speed;
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			cam.m_position -= LibMath::Vector3::normalize(LibMath::Vector3::cross(cam.m_front, cam.m_toTheSky)) * speed;
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			cam.m_position += LibMath::Vector3::normalize(LibMath::Vector3::cross(cam.m_front, cam.m_toTheSky)) * speed;
	}

	void MainManager::processLoadInput(GLFWwindow* window) 
	{
		if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && this->m_inputCoolDown > COOLDOWN)
		{
			std::cout << "Load Locomotive.obj" << '\n';
			this->loadAndAdd(LOCOMOTIVE);
			this->m_scene->m_entities[this->m_scene->m_entities.size() - 1]->m_local *= LibMath::Mat4::Scale({10, 10, 10});
		}
		
		if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && this->m_inputCoolDown > COOLDOWN)
		{
			std::cout << "Load skull.obj" << '\n';
			this->loadAndAdd(SKULL);
			Resources::Texture* newTexture = this->m_manager->createRessource<Resources::Texture>(SKULLTEXTURE);
			this->m_scene->m_entities[this->m_scene->m_entities.size() - 1]->addTexture(newTexture);
		}
		
		if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS && this->m_inputCoolDown > COOLDOWN)
		{
			this->loadAndAdd(BUNNY);
			std::cout << "Load bunny.obj" << '\n';
		} 

		if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS && this->m_inputCoolDown > COOLDOWN)
		{
			std::cout << "Load cube.obj" << '\n';
			this->loadAndAdd(CUBE);
			Resources::Texture* newTexture = this->m_manager->createRessource<Resources::Texture>(CUBETEXTURE);
			this->m_scene->m_entities[this->m_scene->m_entities.size() - 1]->addTexture(newTexture);
		}

		if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS && this->m_inputCoolDown > COOLDOWN)
		{
			std::cout << "Load padoru.obj" << '\n';
			this->loadAndAdd(PADORU);
			Resources::Texture* newTexture = this->m_manager->createRessource<Resources::Texture>(PADORUTEXTURE);
			this->m_scene->m_entities[this->m_scene->m_entities.size() - 1]->addTexture(newTexture);
			this->m_scene->m_entities[this->m_scene->m_entities.size() - 1]->m_local *= LibMath::Mat4::Scale({5, 5, 5});
		}
	}

	void MainManager::loadAndAdd(const char* filename)
	{
		Resources::Model* newModel = this->m_manager->createRessource<Resources::Model>();
		Resources::parseFile(filename, newModel, this->m_monothread, this->m_manager);
		Resources::Entity newEntity(newModel);
		newEntity.m_local *= LibMath::Mat4::Translate(LibMath::Vector3(this->m_scene->m_entities.size() * OFFSET, 0, 0));

		this->m_scene->insert(newEntity);
		this->m_inputCoolDown = 0;
	}

	void MainManager::resetAll() 
	{ 
		this->m_manager->deleteArray();
		this->m_scene->eraseEntities();
		this->m_inputCoolDown = 0;
	}

	void MainManager::closeApp() 
	{
		glfwSetWindowShouldClose(this->m_app->m_screen, true);
		delete this->m_app;
		delete this->m_manager;
		delete this->m_scene;
		delete this->m_shaders;
	}

	MainManager::~MainManager()
	{
		delete this->m_app;
		delete this->m_manager;
		delete this->m_scene;
		delete this->m_shaders;
	}
} // namespace Core
