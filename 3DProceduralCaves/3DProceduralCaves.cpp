#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <array>

#include "stb_image.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "shader.h"
#include "camera.h"
#include "texture.h"
#include "tunnel_mesh.h"
#include "game_object.h"
#include "plane.h"
#include "line.h"
#include "cave_generator.h"

// Global variables
unsigned int screenWidth = 1920;
unsigned int screenHeight = 1080;

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrameTime = 0.0f; // Time of last frame

float lastX = screenWidth / 2.0f, lastY = screenHeight / 2.0f; // last mouse position (center of screen)

bool mouseInputEnabled = true;

Camera viewCamera = Camera(glm::vec3(0.0f, 0.0f, 3.0f));

CaveGenerator caveGenerator = CaveGenerator();
int proceduralStage = 2;

// Function prototypes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void drawImGuiWindow();

void rotateByDegrees(glm::mat4& model, const glm::vec3& rotation)
{
	model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0, 0.0, 0.0));
	model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0, 1.0, 0.0));
	model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0, 0.0, 1.0));
}

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

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
	ImGui_ImplOpenGL3_Init();

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

	// Create rock texture
	stbi_set_flip_vertically_on_load(true); // flip image y-axis, textures 0.0 y-axis is bottom, images is top
	Texture rockTexture = Texture("assets/textures/rock.jpg");

	// shader program stuff
	shaderProgram.use();
	shaderProgram.setInt("textureSampler", 0); // set texture sampler to 0
	
	lineShaderProgram.use();
	lineShaderProgram.setMat4("model", glm::mat4(1.0f));

	caveGenerator.Generate();

	// --------------------------------------
	// RENDER LOOP
	// --------------------------------------
	float my_color[4] = {0.0, 0.0, 0.0, 0.0};
	while (!glfwWindowShouldClose(window))
	{
		// Frame time
		float currentFrameTime = static_cast<float>(glfwGetTime());
		deltaTime = currentFrameTime - lastFrameTime;
		lastFrameTime = currentFrameTime;

		// Poll glfw events
		glfwPollEvents();

		// Draw imgui window
		drawImGuiWindow();

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
		if (proceduralStage >= 2)
		{
			for (TunnelMesh* tunnelMesh : caveGenerator.tunnelMeshes)
			{
				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model, tunnelMesh->GetPosition());
				model = glm::scale(model, tunnelMesh->GetScale());
				rotateByDegrees(model, tunnelMesh->GetRotation());

				tunnelMesh->Draw(shaderProgram, model);
			}
		}
		
		// Draw lines
		if (proceduralStage >= 1)
		{
			lineShaderProgram.use();
			lineShaderProgram.setMat4("view", view);
			lineShaderProgram.setMat4("projection", projection);

			for (Line* line : caveGenerator.drawLines)
			{
				line->Draw();
			}
		}

		// ImGui render
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Swap buffers and check/call events
		glfwSwapBuffers(window);
	}

	// Cleanup
	glDeleteProgram(shaderProgram.ID);

	// Cleanup ImGui
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	// Terminate
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	screenWidth = width, screenHeight = height;
	glViewport(0, 0, width, height); // Set viewport with new size again
}

bool shouldResetMousePos = true;
float nextMouseToggleTime = 0.0f;
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
	
	// Mouse focus toggle
	if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS && glfwGetTime() >= nextMouseToggleTime)
	{
		nextMouseToggleTime = glfwGetTime() + 0.5f;

		mouseInputEnabled = not mouseInputEnabled;

		// reset mouse position if toggling mouse off
		if (not mouseInputEnabled)
			shouldResetMousePos = true;

		glfwSetInputMode(window, GLFW_CURSOR, mouseInputEnabled ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
	}	

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

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	// pass mouse positions to imgui
	ImGuiIO& io = ImGui::GetIO();
	io.MousePos = ImVec2(xpos, ypos);

	if (!mouseInputEnabled) return;

	// prevents snapping when first entering window
	if (shouldResetMousePos)
	{
		lastX = xpos;
		lastY = ypos;
		shouldResetMousePos = false;
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
	if (!mouseInputEnabled) return;

	// adjust fov on scroll
	viewCamera.setFOV(viewCamera.getFOV() + -2.0f * (float)yoffset);
}

void switchProceduralLevel(int newLevel)
{
	proceduralStage = newLevel;
}

std::array<std::string, 4> proceduralLevelNames = {
	"Nothing",
	"LSystem",
	"Meshes",
	"Perlin Noise"
};

float floatParam = 0.0f;
bool boolParam = false;
void drawImGuiWindow()
{
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// Begin window
	ImGui::Begin("Program Parameters", NULL, ImGuiWindowFlags_MenuBar);

	// Begin menu bar
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("Options"))
		{
			if (ImGui::MenuItem("Option 1", "Ctrl+O")) {}
			if (ImGui::MenuItem("Option 2", "Ctrl+1")) {}
			if (ImGui::MenuItem("Option 3", "Ctrl+2")) {}
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	// Procedural levels
	ImGui::TextColored(ImVec4(1, 1, 1, 1), "Procedural levels:");

	for (int i = 0; i < proceduralLevelNames.size(); i++)
	{
		std::string buttonText = std::to_string(i) + " - " + proceduralLevelNames[i] + (proceduralStage == i ? " (Current)" : "");
		if (ImGui::Button(buttonText.c_str()))
			switchProceduralLevel(i);
	}

	// Generation parameters
	ImGui::TextColored(ImVec4(1, 1, 1, 1), "Generation paramaters:");

	ImGui::Checkbox("Checkbox", &boolParam);
	ImGui::SliderFloat("Slider", &floatParam, 0.0f, 1.0f);

	if (ImGui::Button("Generate"))
		caveGenerator.Generate();

	// Key hint
	ImGui::TextColored(ImVec4(1, 1, 0, 1), "Press [Alt] to show cursor.");

	// End window
	ImGui::End();
}