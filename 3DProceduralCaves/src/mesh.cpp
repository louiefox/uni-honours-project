#include "mesh.h"
#include "line.h"

Mesh::Mesh()
{

}

Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &mVAO);
	glDeleteBuffers(1, &mVBO);
	glDeleteBuffers(1, &mEBO);

	for (int i = 0; i < mNormalLines.size(); i++)
		delete mNormalLines[i];
}

void Mesh::setVertices(const std::vector<Vertex>& newVertices)
{
	mVertices.clear();
	mIndicies.clear();

	for (int i = 0; i < newVertices.size(); i++)
		addVertex(newVertices[i].Position, newVertices[i].TextureCoords);
}

void Mesh::addVertex(const glm::vec3& position, const glm::vec2 uvCoords)
{
	int foundIndex = findVertex(position, uvCoords);
	if (foundIndex != -1)
	{
		mIndicies.push_back(foundIndex);
		return;
	}

	mVertices.push_back(Vertex{ position, uvCoords });
	mIndicies.push_back(mVertices.size() - 1);
}

bool Mesh::compareFloats(float value1, float value2)
{
	return std::abs(value1 - value2) < std::numeric_limits<float>::epsilon();
}

int Mesh::findVertex(const glm::vec3& position, const glm::vec2 uvCoords)
{
	for (int i = 0; i < mVertices.size(); i++)
	{
		const Vertex& otherVertex = mVertices[i];

		if (!compareFloats(position.x, otherVertex.Position.x) || !compareFloats(position.y, otherVertex.Position.y) || !compareFloats(position.z, otherVertex.Position.z))
			continue;

		// TODO: Fix - disabled until duplicate vertices are fixed
		//if (!compareFloats(uvCoords.x, otherVertex.TextureCoords.x) || !compareFloats(uvCoords.y, otherVertex.TextureCoords.y))
		//	continue;

		return i;
	}

	return -1;
}

std::vector<Vertex> Mesh::getAllVertices() const
{
	std::vector<Vertex> allVertices;
	for (auto index : mIndicies)
		allVertices.push_back(mVertices[index]);

	return allVertices;
}

// TODO: Calculate normals based on surrounding tunnel meshes
glm::vec3 Mesh::calculateNormalFace(const glm::vec3& vec1, const glm::vec3& vec2, const glm::vec3& vec3)
{
	// cross product of any 2 edges of the face
	return glm::normalize(glm::cross(vec2 - vec1, vec3 - vec1));
}

void Mesh::calculateNormals(const glm::vec3& worldPosition, const glm::vec3& worldRotation)
{
	// clear normals incase function is ran again?
	for (int i = 0; i < mVertices.size(); i++)
	{
		mVertices[i].Normal = glm::vec3(0.0f);
	}

	// loop through each face in the mesh (every 3 indicies)
	for (int i = 0; i < mIndicies.size(); i += 3)
	{
		// get 3 vertex indexes
		int index1 = mIndicies[i], index2 = mIndicies[i + 1], index3 = mIndicies[i + 2];

		glm::vec3 normal = calculateNormalFace(mVertices[index1].Position, mVertices[index2].Position, mVertices[index3].Position);

		mVertices[index1].Normal += normal;
		mVertices[index2].Normal += normal;
		mVertices[index3].Normal += normal;
	}

	// normalize normals
	for (int i = 0; i < mVertices.size(); i++)
	{
		mVertices[i].Normal = glm::normalize(mVertices[i].Normal);
	}

	// create normal lines
	for (int i = 0; i < mVertices.size(); i++) 
	{
		glm::vec3 startPos = mVertices[i].Position;
		glm::vec3 endPos = mVertices[i].Position + 0.1f * mVertices[i].Normal;

		mNormalLines.push_back(new Line(startPos, endPos));
	}
}

void Mesh::generate()
{
	if (mGenerated)
		return;

	mGenerated = true;

	// Create objects
	glGenVertexArrays(1, &mVAO);
	glGenBuffers(1, &mVBO);
	glGenBuffers(1, &mEBO);

	glBindVertexArray(mVAO); // bind VAO before vertex buffers and attributes

	glBindBuffer(GL_ARRAY_BUFFER, mVBO); // bind vertex buffer

	// pass address for first Vertex struct in vertices, they're sequential so the stride/offsets will function normally
	glBufferData(GL_ARRAY_BUFFER, mVertices.size() * sizeof(Vertex), &mVertices[0], GL_STATIC_DRAW); // GL_STATIC_DRAW means data will not change, GL_DYNAMIC_DRAW would store the vertices in faster memory so the data can be changed frequenetly

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mIndicies.size() * sizeof(unsigned int), &mIndicies[0], GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0); // attrib index, attrib size, attrib type, false, stride, offset
	glEnableVertexAttribArray(0);

	// texture coords attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TextureCoords)); // offsetof finds the offset of the variable within that struct
	glEnableVertexAttribArray(1);

	// normal attribute
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
	glEnableVertexAttribArray(2);

	// we're done with VBO so unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// unbind VAO as well, prevents other calls modifying our VAO (not that it'll happen)
	glBindVertexArray(0);
}

void Mesh::draw()
{
	glBindVertexArray(mVAO);
	glDrawElements(GL_TRIANGLES, mIndicies.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Mesh::drawNormalLines()
{
	for (int i = 0; i < mNormalLines.size(); i++)
	{
		mNormalLines[i]->Draw();
	}
}