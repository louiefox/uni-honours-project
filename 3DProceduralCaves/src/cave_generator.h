#pragma once

#include <vector>

#include "line.h"
#include "tunnel_mesh.h"

struct LSystemValue
{
	glm::vec2 Position;
	float Rotation;
};

class CaveGenerator
{
public:
	std::vector<Line*> drawLines;
	std::vector<TunnelMesh*> tunnelMeshes;
	int currentIteration = 0;
	bool tempDevEnablePerlin = false;

	CaveGenerator();
	~CaveGenerator();

	void GenerateNext();
	void ReGenerateCurrent();
	void UpdateDraw();
	void SetAdjustAngle(float newAdjustAngle);

private:
	std::string CurrentString = "X";
	float lineLength = 1.0f;
	float adjustAngle = 45.0f;

	const glm::vec2& getLineEndPos(glm::vec2 startPos, float radians);
};