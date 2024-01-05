#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

struct Vertex
{
	glm::vec3 Position;
	glm::vec2 TextureCoords;
};

class Mesh
{
public:
	Mesh();
	~Mesh();

	const std::vector<Vertex>& getVertices() const;
	void setVertices(const std::vector<Vertex>& newVertices);

	void addVertex(const glm::vec3& position, const glm::vec2 uvCoords);
	int findVertex(const glm::vec3& position, const glm::vec2 uvCoords);

	/// <summary>
	/// All vertices including duplicated ones
	/// </summary>
	/// <returns>Vector of vertices</returns>
	std::vector<Vertex> getAllVertices() const;

	void generate();
	void draw();

private:
	unsigned int mVBO, mVAO, mEBO;
	std::vector<Vertex> mVertices;
	std::vector<unsigned int> mIndicies;

	bool mGenerated = false;

	// helper function to compare floats using epsilon
	bool compareFloats(float value1, float value2);
};