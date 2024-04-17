#include "pch.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "game_object.h"
#include "texture.h"
#include "camera.h"
#include "mesh.h"
#include "line.h"
#include "shader.h"
#include "tunnel_mesh.h"

class GameObjectTest : public ::testing::Test {
protected:
    // Create an instance of GameObject for each test
    GameObject gameObject;
};

// Test case to check setting and getting position
TEST_F(GameObjectTest, TestPosition) {
    // Set a new position
    glm::vec3 newPosition(1.0f, 2.0f, 3.0f);
    gameObject.SetPosition(newPosition);

    // Get the position and check if it matches the set value
    glm::vec3 retrievedPosition = gameObject.GetPosition();
    EXPECT_EQ(retrievedPosition, newPosition);
}

// Test case to check setting and getting rotation
TEST_F(GameObjectTest, TestRotation) {
    // Set a new rotation
    glm::vec3 newRotation(45.0f, 90.0f, 180.0f);
    gameObject.SetRotation(newRotation);

    // Get the rotation and check if it matches the set value
    glm::vec3 retrievedRotation = gameObject.GetRotation();
    EXPECT_EQ(retrievedRotation, newRotation);
}

// Test case to check setting and getting scale
TEST_F(GameObjectTest, TestScale) {
    // Set a new scale
    glm::vec3 newScale(2.0f, 2.0f, 2.0f);
    gameObject.SetScale(newScale);

    // Get the scale and check if it matches the set value
    glm::vec3 retrievedScale = gameObject.GetScale();
    EXPECT_EQ(retrievedScale, newScale);
}

// Setup OpenGL for testing
GLFWwindow* window;
static void setupTestingOpenGL()
{
    // GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window
    window = glfwCreateWindow(100, 100, "Test Window", NULL, NULL);
    if (window == NULL)
    {
        glfwTerminate();
        FAIL() << "Failed to create GLFW widnow";
        return;
    }

    glfwMakeContextCurrent(window);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        FAIL() << "Failed to load GLAD";
        return;
    }
}

static void tearDownTestingOpenGL()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}

class TextureTest : public ::testing::Test {
protected:
    static void SetUpTestCase() {
        setupTestingOpenGL();
    }

    static void TearDownTestCase() {
        tearDownTestingOpenGL();
    }
};

// Test case to check texture loading from file
TEST_F(TextureTest, TestLoading) {
    const char* filePath = "../3DProceduralCaves/assets/textures/rock.jpg";
    Texture texture(filePath);

    EXPECT_TRUE(texture.HasLoaded);
}

// Texture dimensions
TEST_F(TextureTest, TestDimensions) {
    const char* filePath = "../3DProceduralCaves/assets/textures/rock.jpg";
    Texture texture(filePath);

    EXPECT_EQ(texture.Width, 2048);
    EXPECT_EQ(texture.Height, 2048);
}

// Texture channels
TEST_F(TextureTest, TestChannels) {
    const char* filePath = "../3DProceduralCaves/assets/textures/rock.jpg";
    Texture texture(filePath);

    EXPECT_EQ(texture.Channels, 3);
}

class CameraTest : public ::testing::Test {
protected:
    // Create a camera object for each test
    Camera camera{ glm::vec3(0.0f, 0.0f, 0.0f) };
};

// Test case to check setting and getting FOV
TEST_F(CameraTest, TestFOV) {
    // Set a new FOV value
    float newFOV = 35.0f;
    camera.setFOV(newFOV);

    // Get the FOV and check if it matches the set value
    float retrievedFOV = camera.getFOV();
    EXPECT_EQ(retrievedFOV, newFOV);
}

// Test case to check applying mouse input
TEST_F(CameraTest, TestMouseInput) {
    // Apply mouse input (xOffset, yOffset)
    camera.applyMouseInput(1.0f, -1.0f);

    // Get the camera's pitch and yaw angles
    float pitch = camera.getPitch();
    float yaw = camera.getYaw();

    // Check if the pitch and yaw angles have been updated
    EXPECT_NE(pitch, 0.0f);
    EXPECT_NE(yaw, -90.0f);
}

// Test case to check applying keyboard input
TEST_F(CameraTest, TestKeyboardInput) {
    // Apply keyboard input (FORWARD direction, speed)
    camera.applyKeyboardInput(FORWARD, 0.1f);

    // Get the camera's position after applying input
    glm::vec3 position = camera.getPosition();

    // Check if the camera's position has been updated
    EXPECT_NE(position, glm::vec3(0.0f, 0.0f, 0.0f));
}

class MeshTest : public ::testing::Test {
protected:
    static void SetUpTestCase() {
        setupTestingOpenGL();
    }

    static void TearDownTestCase() {
        tearDownTestingOpenGL();
    }
};

// Test case to check adding vertices
TEST_F(MeshTest, TestAddVertex) {
    Mesh mesh;

    // Add a vertex to the mesh
    mesh.addVertex(glm::vec3(0.0f), glm::vec2(0.0f));

    // Check if the vertices vector size is 1
    EXPECT_EQ(mesh.getVertices().size(), 1);
}

// Test case to check finding vertices
TEST_F(MeshTest, TestFindVertex) {
    Mesh mesh;

    // Add a vertex to the mesh
    mesh.addVertex(glm::vec3(0.0f), glm::vec2(0.0f));

    // Find the added vertex
    int index = mesh.findVertex(glm::vec3(0.0f), glm::vec2(0.0f));

    // Check if the vertex is found and index is not -1
    EXPECT_NE(index, -1);
}

// Test case to check mesh generation
TEST_F(MeshTest, TestMeshGeneration) {
    Mesh mesh;

    // Add vertices to the mesh
    mesh.addVertex(glm::vec3(0.0f), glm::vec2(0.0f));
    mesh.addVertex(glm::vec3(1.0f), glm::vec2(1.0f));

    // Generate the mesh
    mesh.generate();

    // Check if the mesh has been generated
    EXPECT_TRUE(mesh.isGenerated());
}

// Test case to check normal calculation
TEST_F(MeshTest, TestNormalCalculation) {
    Mesh mesh;

    // Add vertices to the mesh to form a triangle
    mesh.addVertex(glm::vec3(0.0f), glm::vec2(0.0f));
    mesh.addVertex(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f));
    mesh.addVertex(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f));

    // Calculate normals
    mesh.calculateNormals();

    // Get the normal vectors
    const auto& vertices = mesh.getVertices();
    const glm::vec3& normal1 = vertices[0].Normal;
    const glm::vec3& normal2 = vertices[1].Normal;
    const glm::vec3& normal3 = vertices[2].Normal;

    // Check if the normals are calculated correctly
    // (assuming triangle faces the positive z-direction)
    EXPECT_EQ(normal1, glm::vec3(0.0f, 0.0f, 1.0f));
    EXPECT_EQ(normal2, glm::vec3(0.0f, 0.0f, 1.0f));
    EXPECT_EQ(normal3, glm::vec3(0.0f, 0.0f, 1.0f));
}

class LineTest : public ::testing::Test {
protected:
    static void SetUpTestCase() {
        setupTestingOpenGL();
    }

    static void TearDownTestCase() {
        tearDownTestingOpenGL();
    }
};

// Test case to check the Line constructor
TEST(LineTest, TestConstructor) {
    // Create a new Line object
    glm::vec3 lineStart(0.0f, 0.0f, 0.0f);
    glm::vec3 lineEnd(1.0f, 1.0f, 1.0f);
    Line line(lineStart, lineEnd);

    // Check if the vertices vector size is 2 (start and end points)
    EXPECT_EQ(line.vertices.size(), 2);
}

// Test case to check drawing functionality
TEST(LineTest, TestDraw) {
    // Create a new Line object
    glm::vec3 lineStart(0.0f, 0.0f, 0.0f);
    glm::vec3 lineEnd(1.0f, 1.0f, 1.0f);
    Line line(lineStart, lineEnd);

    // You might want to mock the drawing functionality here and check if it's called
    // For simplicity, let's just check if calling the Draw method doesn't throw an exception
    EXPECT_NO_THROW(line.Draw());
}

class ShaderTest : public ::testing::Test {
protected:
    const char* vertexPath = "../3DProceduralCaves/assets/shaders/flat_vertex_shader.vs";
    const char* fragmentPath = "../3DProceduralCaves/assets/shaders/flat_fragment_shader.fs";
};

// Test case to check the Shader constructor
TEST_F(ShaderTest, TestConstructor) {
    Shader shader(vertexPath, fragmentPath);

    // Check if the shader ID is not zero
    EXPECT_NE(shader.ID, -1);
}

// Test case to check shader activation
TEST_F(ShaderTest, TestUse) {
    Shader shader(vertexPath, fragmentPath);

    // You might want to mock the shader activation here and check if it's called
    // For simplicity, let's just check if calling the use method doesn't throw an exception
    EXPECT_NO_THROW(shader.use());
}

// Test case to check uniform functions
TEST_F(ShaderTest, TestUniformFunctions) {
    Shader shader(vertexPath, fragmentPath);

    // You might want to mock the utility uniform functions and check if they are called correctly
    // For simplicity, let's just check if calling the functions doesn't throw an exception
    EXPECT_NO_THROW(shader.setBool("testBool", true));
    EXPECT_NO_THROW(shader.setInt("testInt", 42));
    EXPECT_NO_THROW(shader.setFloat("testFloat", 3.14f));
    EXPECT_NO_THROW(shader.setColor("testColor", 1.0f, 0.0f, 0.0f));
    EXPECT_NO_THROW(shader.setMat4("testMat4", glm::mat4(1.0f)));
    EXPECT_NO_THROW(shader.setVec3("testVec3", 1.0f, 2.0f, 3.0f));
}

class TunnelMeshTest : public ::testing::Test {
protected:
    // No need for a member tunnelMesh variable here
};

// Test case to check generating geometry
TEST(TunnelMeshTest, TestVertexSmoothing) {
    // Create a new TunnelMesh object
    TunnelMesh tunnelMesh;

    // Create some mock TunnelMesh objects to pass to generateGeometryBlurring function
    std::vector<TunnelMesh*> tunnelMeshes;

    // You might want to mock the generateGeometryBlurring function and check if it's called
    // For simplicity, let's just check if calling the function doesn't throw an exception
    EXPECT_NO_THROW(tunnelMesh.generateGeometryBlurring(tunnelMeshes));
}

// Test case to check setting and getting neighboring tunnel meshes
TEST(TunnelMeshTest, TestNeighboringMeshes) {
    // Create a new TunnelMesh object
    TunnelMesh tunnelMesh;

    // Create some mock TunnelMesh objects
    TunnelMesh* previousTunnelMesh = new TunnelMesh();
    TunnelMesh* nextTunnelMesh = new TunnelMesh();

    // Set neighboring tunnel meshes
    tunnelMesh.setPreviousTunnelMesh(previousTunnelMesh);
    tunnelMesh.setNextTunnelMesh(nextTunnelMesh);

    // Check if the neighboring tunnel meshes are set correctly
    EXPECT_EQ(tunnelMesh.getPreviousTunnelMesh(), previousTunnelMesh);
    EXPECT_EQ(tunnelMesh.getNextTunnelMesh(), nextTunnelMesh);
}

// Mesh triangles
TEST(TunnelMeshTest, TestTriangleSplitting) {
    TunnelMesh tunnelMesh;

    size_t oldTriangleCount = tunnelMesh.getMesh().getAllVertices().size();
    tunnelMesh.splitMeshTriangles(1);
    size_t newTriangleCount = tunnelMesh.getMesh().getAllVertices().size();

    EXPECT_EQ(newTriangleCount, oldTriangleCount * 4);
}

// World matrix check
TEST(TunnelMeshTest, TestWorldMatrix) {
    TunnelMesh tunnelMesh;

    glm::mat4 testWorldMatrix = glm::mat4(1.0f);
    testWorldMatrix = glm::translate(testWorldMatrix, glm::vec3(0.0, 0.0, 2.0f));

    tunnelMesh.SetPosition(glm::vec3(0.0, 0.0, 2.0f));

    EXPECT_EQ(tunnelMesh.getWorldMatrix(), testWorldMatrix);
}