#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <stb/stb_image.h>

#include <iostream>
#include <array>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "shader.h"
#include "camera.h"
#include "texture.h"
#include "tunnel_mesh.h"
#include "game_object.h"
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

int proceduralStage = 2;
int preBlurSplitting = 2;
int postBlurSplitting = 1;
int shaderRenderMode = 1;

bool showMeshHighlight = false;
int currentMeshHighlight = 0;
bool showLightingTest = false;
bool showNormalLines = false;

// Function prototypes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void drawImGuiWindow(CaveGenerator& caveGenerator);

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
	Shader shaderProgram = Shader("assets/shaders/textured_vertex_shader.vs", "assets/shaders/textured_fragment_shader.fs");
	Shader flatShaderProgram = Shader("assets/shaders/flat_vertex_shader.vs", "assets/shaders/flat_fragment_shader.fs");
	Shader smoothShaderProgram = Shader("assets/shaders/smooth_vertex_shader.vs", "assets/shaders/smooth_fragment_shader.fs");
	Shader lineShaderProgram = Shader("assets/shaders/line_vertex_shader.vs", "assets/shaders/line_fragment_shader.fs");
	Shader lightSrcShaderProgram = Shader("assets/shaders/lightsource_vertex_shader.vs", "assets/shaders/lightsource_fragment_shader.fs");

	// Create rock texture
	stbi_set_flip_vertically_on_load(true); // flip image y-axis, textures 0.0 y-axis is bottom, images is top
	Texture rockTexture = Texture("assets/textures/rock.jpg");
	Texture rockHighlightTexture = Texture("assets/textures/rock_highlight.jpg");
	Texture rockHighlightAdjacentTexture = Texture("assets/textures/rock_highlight_adjacent.jpg");

	// shader program stuff
	shaderProgram.use();
	shaderProgram.setInt("textureSampler", 0); // set texture sampler to 0
	
	lineShaderProgram.use();
	lineShaderProgram.setMat4("model", glm::mat4(1.0f));

	CaveGenerator caveGenerator = CaveGenerator();
	caveGenerator.GenerateNext();
	caveGenerator.GenerateNext();

	// TEMP: Lighting stuff
	float vertices[] = {
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
	};

	unsigned int lightingCubeVAO, lightingCubeVBO;
	glGenVertexArrays(1, &lightingCubeVAO);
	glGenBuffers(1, &lightingCubeVBO);

	glBindBuffer(GL_ARRAY_BUFFER, lightingCubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(lightingCubeVAO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);	
	
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glm::vec3 lightSrcPos(0.0f, 0.0f, -3.0f);

	unsigned int lightingSrcVAO;
	glGenVertexArrays(1, &lightingSrcVAO);
	glBindVertexArray(lightingSrcVAO);
	glBindBuffer(GL_ARRAY_BUFFER, lightingCubeVBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// --------------------------------------
	// RENDER LOOP
	// --------------------------------------
	while (!glfwWindowShouldClose(window))
	{
		// Frame time
		float currentFrameTime = static_cast<float>(glfwGetTime());
		deltaTime = currentFrameTime - lastFrameTime;
		lastFrameTime = currentFrameTime;

		// Poll glfw events
		glfwPollEvents();

		// Draw imgui window
		drawImGuiWindow(caveGenerator);

		// Process input
		processInput(window);

		// Rendering commands
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // set background color
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear color buffer and depth buffer

		// Lighting stuff
		//shaderProgram.use();
		//shaderProgram.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
		//shaderProgram.setVec3("lightColor", 1.0f, 1.0f, 1.0f);

		// Textures - bind container texture to first sampler
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, rockTexture.ID);

		// Use shader
		//shaderProgram.use();

		// Transformations
		glm::mat4 view = viewCamera.getViewMatrix(); // view matrix using camera data and look at function
		//shaderProgram.setMat4("view", view);

		glm::mat4 projection = glm::perspective(glm::radians(viewCamera.getFOV()), (float)screenWidth / (float)screenHeight, 0.1f, 100.0f); // projection matrix using perspective
		//shaderProgram.setMat4("projection", projection);

		Shader& mainShader = shaderRenderMode == 0 ? shaderProgram : (shaderRenderMode == 1 ? smoothShaderProgram : flatShaderProgram);

		// lighting shader
		mainShader.use();
		mainShader.setVec3("objectColor", 1.0f, 0.4f, 0.4f);
		mainShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
		mainShader.setVec3("lightPos", lightSrcPos.x, lightSrcPos.y, lightSrcPos.z);
		mainShader.setVec3("viewPos", viewCamera.getPosition().x, viewCamera.getPosition().y, viewCamera.getPosition().z);

		mainShader.setMat4("view", view);
		mainShader.setMat4("projection", projection);

		// Textures - bind container texture to first sampler
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, rockTexture.ID);

		// Draw meshes
		if (proceduralStage >= 2)
		{
			TunnelMesh* highlightedMesh = caveGenerator.tunnelMeshes[currentMeshHighlight];

			for (int i = 0; i < caveGenerator.tunnelMeshes.size(); i++)
			{
				TunnelMesh* tunnelMesh = caveGenerator.tunnelMeshes[i];

				// texture stuff
				glActiveTexture(GL_TEXTURE0);

				if(showMeshHighlight && tunnelMesh == highlightedMesh)
					glBindTexture(GL_TEXTURE_2D, rockHighlightTexture.ID);
				else if(showMeshHighlight && (tunnelMesh == highlightedMesh->getPreviousTunnelMesh() || tunnelMesh == highlightedMesh->getNextTunnelMesh() || tunnelMesh == highlightedMesh->getNextTunnelMesh2()))
					glBindTexture(GL_TEXTURE_2D, rockHighlightAdjacentTexture.ID);
				else
					glBindTexture(GL_TEXTURE_2D, rockTexture.ID);

				// draw
				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model, tunnelMesh->GetPosition());
				model = glm::scale(model, tunnelMesh->GetScale());
				rotateByDegrees(model, tunnelMesh->GetRotation());

				mainShader.setMat4("model", model);
				tunnelMesh->draw();
			}			
		}
		
		// Draw lines
		if(showNormalLines)
		{
			lineShaderProgram.use();
			lineShaderProgram.setMat4("view", view);
			lineShaderProgram.setMat4("projection", projection);

			for (const auto& tunnelMesh : caveGenerator.tunnelMeshes)
			{
				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model, tunnelMesh->GetPosition());
				model = glm::scale(model, tunnelMesh->GetScale());
				rotateByDegrees(model, tunnelMesh->GetRotation());

				lineShaderProgram.setMat4("model", model);
				tunnelMesh->drawNormalLines();
			}
		}

		if (proceduralStage == 1)
		{
			lineShaderProgram.use();
			lineShaderProgram.setMat4("view", view);
			lineShaderProgram.setMat4("projection", projection);

			for (Line* line : caveGenerator.drawLines)
			{
				line->Draw();
			}
		}

		if (showLightingTest)
		{
			// Draw lighting source
			lightSrcShaderProgram.use();
			lightSrcShaderProgram.setMat4("view", view);
			lightSrcShaderProgram.setMat4("projection", projection);

			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, lightSrcPos);
			model = glm::scale(model, glm::vec3(0.2f));
			lightSrcShaderProgram.setMat4("model", model);

			glBindVertexArray(lightingSrcVAO);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		// ImGui render
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Swap buffers and check/call events
		glfwSwapBuffers(window);
	}

	// Cleanup
	glDeleteProgram(shaderProgram.ID);
	glDeleteProgram(flatShaderProgram.ID);
	glDeleteProgram(smoothShaderProgram.ID);
	glDeleteProgram(lineShaderProgram.ID);
	glDeleteProgram(lightSrcShaderProgram.ID);

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
	const float cameraSpeed = static_cast<float>((isShiftPressed ? 5.0f : 1.0f) * deltaTime); // adjust accordingly
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

void switchProceduralLevel(CaveGenerator& caveGenerator, int newLevel)
{
	proceduralStage = newLevel;

	caveGenerator.tempDevEnablePerlin = proceduralStage == 3;
	caveGenerator.ReGenerateCurrent();
}

std::array<std::string, 4> proceduralLevelNames = {
	"Nothing",
	"LSystem",
	"Meshes",
	"Perlin Noise"
};

//std::array<std::string, 3> renderModes = {
//	"Textured",
//	"Smooth Shading",
//	"Flat Shading"
//};

float floatParam = 45.0f;
void drawImGuiWindow(CaveGenerator& caveGenerator)
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
			switchProceduralLevel(caveGenerator, i);
	}

	// Generation parameters
	ImGui::TextColored(ImVec4(1, 1, 1, 1), "Generation paramaters:");
	
	if (ImGui::SliderFloat("Branch Angle", &floatParam, 0.0f, 90.0f))
		caveGenerator.SetAdjustAngleRange(floatParam);
	
	if (ImGui::SliderInt("Pre Blur Splitting", &preBlurSplitting, 0, 10))
		caveGenerator.SetPreBlurSplitting(preBlurSplitting);
	
	if (ImGui::SliderInt("Post Blur Splitting", &postBlurSplitting, 0, 10))
		caveGenerator.SetPostBlurSplitting(postBlurSplitting);

	if (ImGui::Button("Regenerate Current"))
		caveGenerator.ReGenerateCurrent();	
	
	ImGui::SameLine();
	if (ImGui::Button("Generate Next"))
		caveGenerator.GenerateNext();

	//ImGui::ProgressBar(0.5f);

	// Polygon modes
	ImGui::TextColored(ImVec4(1, 1, 1, 1), "Polygon render modes:");

	if (ImGui::Button("0 - Fill"))
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);	
	
	ImGui::SameLine();
	if (ImGui::Button("1 - Line"))
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);	
	
	ImGui::SameLine();
	if (ImGui::Button("2 - Point"))
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);

	// Shading modes
	ImGui::TextColored(ImVec4(1, 1, 1, 1), "Render modes:");

	//if (ImGui::BeginCombo("Mode", renderModes[shaderRenderMode].c_str(), ImGuiComboFlags_NoArrowButton))
	//{
	//	for (int i = 0; i < renderModes.size(); i++)
	//	{
	//		bool isSelected = shaderRenderMode == i;

	//		if (ImGui::Selectable(renderModes[i].c_str(), isSelected))
	//			shaderRenderMode = i;

	//		if (isSelected)
	//			ImGui::SetItemDefaultFocus();
	//	}

	//	ImGui::EndCombo();
	//}

	if (ImGui::Button("0 - Textured"))
		shaderRenderMode = 0;

	ImGui::SameLine();
	if (ImGui::Button("1 - Smooth Shading"))
		shaderRenderMode = 1;

	ImGui::SameLine();
	if (ImGui::Button("2 - Flat Shading"))
		shaderRenderMode = 2;

	// Other stuff
	ImGui::TextColored(ImVec4(1, 1, 1, 1), "Tunnel section debug:");

	ImGui::Checkbox("Highlight tunnel mesh", &showMeshHighlight);
	ImGui::SliderInt("Mesh ID", &currentMeshHighlight, 0, caveGenerator.tunnelMeshes.size() - 1);

	// Lighting
	ImGui::TextColored(ImVec4(1, 1, 1, 1), "Lighting");

	ImGui::Checkbox("Show light source", &showLightingTest);
	ImGui::Checkbox("Show normal lines", &showNormalLines);

	// Key hint
	ImGui::TextColored(ImVec4(1, 1, 0, 1), "Press [Alt] to show cursor.");

	// End window
	ImGui::End();
}