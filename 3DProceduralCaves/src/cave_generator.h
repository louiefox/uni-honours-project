#pragma once

#include <vector>
#include <string>
#include <array>

#include "line.h"
#include "tunnel_mesh.h"

struct LSystemValue
{
	glm::vec2 Position;
	float Rotation;
	float AdjustAngle;
	TunnelMesh* LastIntersection;
};

class CaveGenerator
{
public:
	std::vector<Line*> drawLines;
	std::vector<TunnelMesh*> tunnelMeshes;
	int currentIteration = 0;
	int proceduralStage = 4;

	CaveGenerator();
	~CaveGenerator();

	void GenerateNext();
	void ReGenerateCurrent(int currentMeshHighlight);
	void UpdateDraw();
	void SetRandomSeed(std::string newSeed) { randomSeed = std::stoi(newSeed); }
	void SetAdjustAngleRange(float newRange) { adjustAngleRange = newRange; }
	void SetPreBlurSplitting(int newValue) { preBlurSplitting = newValue; }
	void SetPostBlurSplitting(int newValue) { postBlurSplitting = newValue; }
	void SetNoiseStrengths(std::array<float, 3> newValues) { perlinNoiseStrengths = newValues; }

	float GetRandomSeed() const { return randomSeed; }
	std::array<float, 3> GetNoiseStrengths() const { return perlinNoiseStrengths; }

private:
	std::string CurrentString = "X";
	unsigned int randomSeed = 0;
	float lineLength = 1.0f;
	float adjustAngleRange = 20.0f;
	float preBlurSplitting = 2;
	float postBlurSplitting = 3;
	std::array<float, 3> perlinNoiseStrengths;
	int currentMeshHighlight = -1;

	const glm::vec2& getLineEndPos(glm::vec2 startPos, float radians);
	void calculateMeshNormals();
};