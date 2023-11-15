#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "shader.h"
#include "camera.h"
#include "texture.h"
#include "plane.h"
#include "line.h"
#include "stb_image.h"

// Global variables
unsigned int screenWidth = 1920;
unsigned int screenHeight = 1080;

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrameTime = 0.0f; // Time of last frame

float lastX = screenWidth / 2.0f, lastY = screenHeight / 2.0f; // last mouse position (center of screen)

Camera viewCamera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));

// Function prototypes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

int main()
{
	// Initialize GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create window
	GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "3D Procedural Caves", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW widnow" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	// Initialize GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// Set viewport and callback for window size changed
	glViewport(0, 0, screenWidth, screenHeight);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// Enable depth testing
	glEnable(GL_DEPTH_TEST);

	// Enable mouse input
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set mouse input callback function
	glfwSetCursorPosCallback(window, mouse_callback);

	// Set scroll input callback function
	glfwSetScrollCallback(window, scroll_callback);

	// Create shader
	Shader shaderProgram = Shader("assets/shaders/vertex_shader.vs", "assets/shaders/fragment_shader.fs");
	Shader lineShaderProgram = Shader("assets/shaders/line_vertex_shader.vs", "assets/shaders/line_fragment_shader.fs");

	// Create container texture
	stbi_set_flip_vertically_on_load(true); // flip image y-axis, textures 0.0 y-axis is bottom, images is top
	Texture rockTexture = Texture("assets/textures/rock.jpg");

	// --------------------------------------
	// CREATE CUBE
	// --------------------------------------
	float vertices[] = {
		// positions		  // texture coords
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
		0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
		0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
		0.5f, -0.5f, 0.5f, 0.0f, 0.0f,

		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		0.5f, -0.5f, -0.5f,  1.0f, 1.0f,

		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f
	};	

	unsigned int indicies[] = {
		0, 1, 2, 
		2, 3, 0,		
		
		4, 5, 6, 
		6, 7, 4,		
		
		8, 9, 10, 
		10, 4, 8,		
		
		11, 12, 13,
		13, 14, 11,
		
		10, 16, 5, 
		5, 4, 10,		
		
		3, 2, 15, 
		15, 17, 3
	};

	unsigned int VBO, VAO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO); // bind VAO before vertex buffers and attributes

	glBindBuffer(GL_ARRAY_BUFFER, VBO); // bind vertex buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // GL_STATIC_DRAW means data will not change, GL_DYNAMIC_DRAW would store the vertices in faster memory so the data can be changed frequenetly

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indicies), indicies, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0); // attrib index, attrib size, attrib type, false, stride, offset
	glEnableVertexAttribArray(0);

	// texture coords attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// we're done with VBO so unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// unbind VAO as well, prevents other calls modifying our VAO (not that it'll happen)
	glBindVertexArray(0);

	// shader program stuff
	shaderProgram.use();
	shaderProgram.setInt("textureSampler", 0); // set texture sampler to 0

	// rectangles to draw - currently using 1, just saving this for later
	glm::vec3 cubePositions[] = {
		glm::vec3(0.0f,  -1.0f,  0.0f),
		glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(0.0f,  1.0f,  0.0f),
		glm::vec3(0.0f,  2.0f,  0.0f)
	};

	Plane planesToDraw[] = {
		Plane(1),
		Plane(4),
		Plane(9),
		Plane(81)
	};	
	
	Line linesToDraw[] = {
		Line(glm::vec3(0.0f,  0.0f,  0.0f), glm::vec3(5.0f,  0.0f,  5.0f))
	};

	lineShaderProgram.use();
	lineShaderProgram.setMat4("model", glm::mat4(1.0f));

	// --------------------------------------
	// RENDER LOOP
	// --------------------------------------
	while (!glfwWindowShouldClose(window))
	{
		// Frame time
		float currentFrameTime = static_cast<float>(glfwGetTime());
		deltaTime = currentFrameTime - lastFrameTime;
		lastFrameTime = currentFrameTime;

		// Process input
		processInput(window);

		// Rendering commands
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // set background color
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear color buffer and depth buffer

		// Textures - bind container texture to first sampler
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, rockTexture.ID);

		// Use shader
		shaderProgram.use();

		// Transformations
		glm::mat4 view = viewCamera.getViewMatrix(); // view matrix using camera data and look at function
		shaderProgram.setMat4("view", view);

		glm::mat4 projection = glm::perspective(glm::radians(viewCamera.getFOV()), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f); // projection matrix using perspective
		shaderProgram.setMat4("projection", projection);

		// Draw planes
		int planeIndex = 0;
		for (Plane& plane : planesToDraw)
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[planeIndex]);
			//model = glm::scale(model, glm::vec3(2.0, 2.0, 10.0));

			shaderProgram.setMat4("model", model);

			plane.Draw();

			planeIndex++;
		}
		
		// Draw lines
		lineShaderProgram.use();
		lineShaderProgram.setMat4("view", view);
		lineShaderProgram.setMat4("projection", projection);

		for (Line& line : linesToDraw)
		{
			line.Draw();
		}

		// Swap buffers and check/call events
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Cleanup
	glDeleteProgram(shaderProgram.ID);

	// Terminate
	glfwTerminate();
	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	screenWidth = width, screenHeight = height;
	glViewport(0, 0, width, height); // Set viewport with new size again
}

void processInput(GLFWwindow* window)
{
	// Window close input
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	// Testing polygon render modes - add keys to switch between smooth/flat shading
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);

	// Camera input
	const bool isShiftPressed = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;
	const float cameraSpeed = static_cast<float>((isShiftPressed ? 20.0f : 5.0f) * deltaTime); // adjust accordingly
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		viewCamera.applyKeyboardInput(FORWARD, cameraSpeed);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		viewCamera.applyKeyboardInput(FORWARD, -cameraSpeed);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		viewCamera.applyKeyboardInput(RIGHT, cameraSpeed);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		viewCamera.applyKeyboardInput(RIGHT, -cameraSpeed);
}

bool firstMouse = true;
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	// prevents snapping when first entering window
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	// find offsets since last mouse move
	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates range from bottom to top
	lastX = xpos, lastY = ypos;

	// adjust camera angles by offsets
	viewCamera.applyMouseInput(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	// adjust fov on scroll
	viewCamera.setFOV(viewCamera.getFOV() + -2.0f * (float)yoffset);
}