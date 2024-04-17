#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

#include "line.h"

struct Vertex
{
	glm::vec3 Position;
	glm::vec2 TextureCoords;
	glm::vec3 Normal;
};

class Mesh
{
public:
	Mesh();
	~Mesh();

	const std::vector<Vertex>& getVertices() const { return mVertices; }
	std::vector<Vertex>& getVertices() { return mVertices; }
	void setVertices(const std::vector<Vertex>& newVertices);

	const std::vector<unsigned int>& getIndicies() const { return mIndicies; }

	void addVertex(const glm::vec3& position, const glm::vec2 uvCoords);
	int findVertex(const glm::vec3& position, const glm::vec2 uvCoords);

	/// <summary>
	/// All vertices including duplicated ones
	/// </summary>
	/// <returns>Vector of vertices</returns>
	std::vector<Vertex> getAllVertices() const;

	void generate();
	void calculateNormals();
	void createNormalLines();
	void draw();
	void drawNormalLines();

	bool isGenerated() { return mGenerated; }

private:
	unsigned int mVBO, mVAO, mEBO;
	std::vector<Vertex> mVertices;
	std::vector<unsigned int> mIndicies;
	std::vector<Line*> mNormalLines;

	bool mGenerated = false;

	// helper function to compare floats using epsilon
	bool compareFloats(float value1, float value2);

	// normal face function
	glm::vec3 calculateNormalFace(const glm::vec3& vec1, const glm::vec3& vec2, const glm::vec3& vec3);
};