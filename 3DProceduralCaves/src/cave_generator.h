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
	std::vector<TunnelMesh*> tunnelIntersectionMeshes;
	int currentIteration = 0;
	bool tempDevEnablePerlin = false;

	CaveGenerator();
	~CaveGenerator();

	void GenerateNext();
	void ReGenerateCurrent();
	void UpdateDraw();
	void SetAdjustAngleRange(float newRange) { adjustAngleRange = newRange; }
	void SetPreBlurSplitting(int newValue) { preBlurSplitting = newValue; }
	void SetPostBlurSplitting(int newValue) { postBlurSplitting = newValue; }

private:
	std::string CurrentString = "X";
	float lineLength = 1.0f;
	float adjustAngleRange = 45.0f;
	float preBlurSplitting = 2;
	float postBlurSplitting = 1;

	const glm::vec2& getLineEndPos(glm::vec2 startPos, float radians);
	void calculateMeshNormals();
};