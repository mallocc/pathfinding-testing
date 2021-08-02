#pragma once

#include "opengl.h"
#include "glm.h"
#include <chrono>
#include <thread>

//Error callback  
static void		error_callback(int error, const char* description)
{
	fputs(description, stderr);
	_fgetchar();
}

struct Camera
{
	glm::vec3 pos, vel, dir, dir_vel, up, up_vel;

	Camera() {}

	Camera(glm::vec3 pos, glm::vec3 vel, glm::vec3 dir, glm::vec3 up)
	{
		this->pos = pos;
		this->vel = vel;
		this->dir = glm::normalize(dir);
		this->up = up;
	}

	void update(float dt, float friction)
	{
		vel *= friction;
		pos += vel * dt;
		dir_vel *= friction;
		dir += dir_vel;
		up_vel *= friction;
		up += up_vel;
	}

	void apply_force(glm::vec3 force)
	{
		vel += force;
	}
	void apply_force_foward()
	{
		vel += dir;
	}
	void apply_force_foward(float mag)
	{
		vel += dir * mag;
	}
	void apply_force_backward()
	{
		vel -= dir;
	}
	void apply_force_right()
	{
		vel += glm::cross(dir, up);
	}
	void apply_force_left()
	{
		vel += -glm::cross(dir, up);
	}

	void yaw_left(float amount)
	{
		dir = glm::quat(glm::vec3(0, 1, 0) * glm::radians(amount)) * dir;
	}
	void yaw_right(float amount)
	{
		dir = glm::quat(glm::vec3(0, -1, 0) * glm::radians(amount)) * dir;
	}
	void pitch_up(float amount)
	{
		glm::vec3 right = glm::cross(dir, up);
		up = glm::quat(right * glm::radians(amount)) * up;
		dir = glm::quat(right * glm::radians(amount)) * dir;
	}
	void pitch_down(float amount)
	{
		glm::vec3 right = -glm::cross(dir, up);
		up = glm::quat(right * glm::radians(amount)) * up;
		dir = glm::quat(right * glm::radians(amount)) * dir;
	}
	void roll_left(float amount);
	void roll_right(float amount);

	void brake(float friction)
	{
		vel *= friction;
	}

	glm::vec3 get_position()
	{
		return pos;
	}

	glm::vec3 get_look_position()
	{
		return pos + dir;
	}

	glm::vec3 get_up()
	{
		return up;
	}
};


typedef void(*GLContentLoop)();
typedef void(*GLContentInit)();

struct GLContent
{
private:
	glm::vec2
		window_size = glm::vec2(1280,720);

	glm::vec3
		eye_pos = glm::vec3(0, 0, -5),
		eye_look_pos,
		up = glm::vec3(0, 1, 0);

	glm::vec4 clear_color;

	glm::mat4
		model,
		view,
		projection;

	float
		fov = 45.0f,
		aspect_ratio = window_size.x / window_size.y,
		near_z = 0.1f,
		far_z = 1000.0f,
		isometric_depth = 5.0f;


	glm::mat4 getOrtho()
	{
		return glm::ortho(
			0.0f			+ eye_pos.x - window_size.x / 2.0f, 
			window_size.x	+ eye_pos.x - window_size.x / 2.0f,
			0.0f			+ eye_pos.y - window_size.y / 2.0f,
			window_size.y	+ eye_pos.y - window_size.y / 2.0f,  
			-far_z, far_z);
		//return glm::perspective(glm::radians(1.0f), 1.0f, 0.1f, 100.0f * window_size.x);
	}
	glm::mat4 getOrthoView()
	{
		return glm::mat4(1.0f);
		/*return glm::lookAt(
			glm::vec3(0.5f * window_size.x, 0.5f * window_size.y, 57.28996f * window_size.x), 
			glm::vec3(0.5f * window_size.x, 0.5f * window_size.y, 0), 
			glm::vec3(0, 1, 0));*/
	}

	glm::mat4 getOrthoIsometric()
	{
		return glm::ortho(0.0f, window_size.x, 0.0f, window_size.y, -far_z, far_z);
		//return glm::perspective(glm::radians(1.0f), 1.0f, 0.1f, 100.0f * window_size.x);
	}
	glm::mat4 getOrthoIsometricView()
	{
		return glm::lookAt(eye_pos, eye_look_pos, up);
	}

	glm::mat4 getPseudoIsometric()
	{
		return glm::perspective(glm::radians(2.864f), aspect_ratio, near_z, far_z);
	}
	glm::mat4 getPseudoIsometricView()
	{
		return glm::lookAt(eye_pos + (isometric_depth * 20.0f) * glm::vec3(1,1,1), eye_pos, up);
	}

	glm::mat4 getPerspective()
	{
		return glm::perspective(glm::radians(fov), aspect_ratio, near_z, far_z);
	}
	glm::mat4 getPerspectiveView()
	{
		return glm::lookAt(eye_pos, eye_look_pos, up);
	}
	
	glm::mat4 getHyperPerspective()
	{
		return glm::perspective(glm::radians(160.0f), aspect_ratio, near_z, far_z);
	}
	glm::mat4 getHyperPerspectiveView()
	{
		return glm::lookAt(eye_pos, eye_look_pos, up);
	}

	//GL graphics loop
	void			glLoop(GLContentLoop loop, GLFWwindow * window)
	{
		printf("\n");
		printf("Running GL loop...\n");

		//Main Loop  
		do
		{
			// start clock for this tick
			auto start = std::chrono::high_resolution_clock::now();

			loop();

			//Swap buffers  
			glfwSwapBuffers(window);
			//Get and organize events, like keyboard and mouse input, window resizing, etc...  
			glfwPollEvents();

			// stop clock
			auto finish = std::chrono::high_resolution_clock::now();
			int ms = float(std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count());
			long newWait = 5 - ms;// -(gm.gameSpeed);
			newWait = newWait < 0 ? 0 : newWait;
			// throttle the graphics loop to cap at a certain fps
			std::this_thread::sleep_for(std::chrono::milliseconds(newWait));

		} //Check if the ESC or Q key had been pressed or if the window had been closed  
		while (!glfwWindowShouldClose(window));

		printf("\n");
		printf("Window has closed. Application will now exit.\n");

		//Close OpenGL window and terminate GLFW  
		glfwDestroyWindow(window);
		//Finalize and clean up GLFW  
		glfwTerminate();

		exit(EXIT_SUCCESS);
	}
	//GL window initialise
	GLFWwindow *				initWindow(GLContentInit init, GLFWkeyfun key_func, GLFWmousebuttonfun mouse_func)
	{
		GLFWwindow * window;

		//Set the error callback  
		glfwSetErrorCallback(error_callback);

		printf("Initialising GLFW...\n");
		//Initialize GLFW  
		if (!glfwInit())
		{
			exit(EXIT_FAILURE);
		}
#ifdef __APPLE__
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif

		printf("Creating window...\n");
		//Create a window and create its OpenGL context  
		window = glfwCreateWindow(window_size.x, window_size.y, "Test Window", NULL, NULL);
		//If the window couldn't be created  
		if (!window)
		{
			fprintf(stderr, "Failed to open GLFW window.\n");
			glfwTerminate();
			exit(EXIT_FAILURE);
		}

		printf("Setting window context...\n");
		//This function makes the context of the specified window current on the calling thread.   
		glfwMakeContextCurrent(window);

		//Sets the key callback  
		glfwSetKeyCallback(window, key_func);
		glfwSetMouseButtonCallback(window, mouse_func);

		printf("Initialising GLEW...\n");
		//Initialize GLEW  
		GLenum err = glewInit();
		//If GLEW hasn't initialized  
		if (err != GLEW_OK)
		{
			fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
			glfwTerminate();
			exit(EXIT_FAILURE);
		}

		// Enable depth test
		//glEnable(GL_DEPTH_TEST);
		// Accept fragment if it closer to the camera than the former one
		//glDepthFunc(GL_LEQUAL);
		// Cull triangles which normal is not towards the camera
		//glEnable(GL_CULL_FACE);
		// enable texturineg
		glEnable(GL_TEXTURE_2D);

		glEnable(GL_BLEND);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		// init
		init();

		return window;
	}

public:

	GLContent() {}

	GLContent(glm::vec3 window_size, glm::vec3 eye_pos, glm::vec3 eye_look_pos, glm::vec3 up, float fov, float aspect_ratio, float near_z, float far_z)
	{
		this->window_size = window_size;
		this->eye_pos = eye_pos;
		this->eye_look_pos = eye_look_pos;
		this->up = up;
		this->fov = fov;
		this->aspect_ratio = aspect_ratio;
		this->near_z = near_z;
		this->far_z = far_z;
	}

	
	void run(GLContentLoop loop, GLContentInit init, GLFWkeyfun key_func, GLFWmousebuttonfun mouse_func)
	{
		glLoop(loop, initWindow(init, key_func, mouse_func));
	}

	void loadPerspective()
	{
		view = getPerspectiveView();
		projection = getPerspective();
	}

	void loadOrtho()
	{
		view = getOrthoView();
		projection = getOrtho();
	}

	void loadPseudoIsometric()
	{
		view = getPseudoIsometricView();
		projection = getPseudoIsometric();
	}

	void loadHyperPerspective()
	{
		view = getHyperPerspectiveView();
		projection = getHyperPerspective();
	}

	glm::vec3 * get_eye_pos()
	{
		return &eye_pos;
	}

	glm::vec3 * get_eye_look_pos()
	{
		return &eye_look_pos;
	}

	glm::vec3 * get_up()
	{
		return &up;
	}

	glm::mat4 * get_model_mat()
	{
		return &model;
	}
	glm::mat4 * get_view_mat()
	{
		return &view;
	}
	glm::mat4 * get_proj_mat()
	{
		return &projection;
	}

	void set_clear_color(glm::vec3 color)
	{
		clear_color = glm::vec4(color, 1.0f);
	}

	void set_clear_color(glm::vec4 color)
	{
		clear_color = color;
	}

	void clearAlpha()
	{
		glClearColor(0.0f,0.0f,0.0f,0.0f);
	}
	void clearDepthBuffer()
	{
		glClear(GL_DEPTH_BUFFER_BIT);
	}
	void clearColorBuffer()
	{
		glClear(GL_COLOR_BUFFER_BIT);
	}
	void clearColor()
	{
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
	}
	void clearBuffers()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	void clearAll()
	{
		clearBuffers();
		clearColor();
	}
	
	glm::vec2 get_window_size()
	{
		return window_size;
	}

	void set_eye_pos(glm::vec3 pos)
	{
		eye_pos = pos;
	}

	void set_eye_look_pos(glm::vec3 pos)
	{
		eye_look_pos = pos;
	}

	void set_up(glm::vec3 up)
	{
		this->up = up;
	}

	void set_camera(Camera * camera)
	{
		eye_pos = camera->pos;
		eye_look_pos = camera->dir + camera->pos;
		up = camera->up;
	}

	float get_isometric_depth()
	{
		return isometric_depth;
	}

	void set_isometric_depth(float depth)
	{
		isometric_depth = depth;
	}

};




