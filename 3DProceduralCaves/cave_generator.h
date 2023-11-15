#pragma once

#include <vector>
#include "line.h"

struct LSystemValue
{
	glm::vec2 Position;
	float Rotation;
};

class CaveGenerator
{
public:
	std::vector<Line*> drawLines;
	int currentIteration = 0;

	CaveGenerator()
	{

	}

	~CaveGenerator()
	{
		for (Line* line : drawLines)
			delete line;
	}

	void Generate()
	{
		std::string nextString = "";
		for (int i = 0; i < CurrentString.length(); i++)
		{
			switch (CurrentString[i])
			{
				case 'X':
					nextString += "F+[[X]-X]-F[-FX]+X";
					break;				
				case 'F':
					nextString += "FF";
					break;
				default:
					nextString += CurrentString[i];
			}
		}

		CurrentString = nextString;

		currentIteration++;
		std::cout << "Iteration " << currentIteration << std::endl;
		std::cout << CurrentString << std::endl;

		UpdateDraw();
	}

	void UpdateDraw()
	{
		for (Line* line : drawLines)
			delete line;

		drawLines = {};

		LSystemValue currentValue = LSystemValue{ glm::vec2(0.0f, 0.0f), 0.0f };
		std::vector<LSystemValue> valueStack;
		for (int i = 0; i < CurrentString.length(); i++)
		{
			switch (CurrentString[i])
			{
				case 'F':
					glm::vec2 endPos = getLineEndPos(currentValue.Position, currentValue.Rotation);

					// create new line mesh, swap x/y around and invert z so tree is facing forward
					drawLines.push_back(new Line(glm::vec3(currentValue.Position.y, 0.0f, -currentValue.Position.x), glm::vec3(endPos.y, 0.0f, -endPos.x)));

					currentValue.Position = endPos;
					break;
				case '-':
					currentValue.Rotation -= adjustAngle;
					break;				
				case '+':
					currentValue.Rotation += adjustAngle;
					break;
				case '[':
					valueStack.push_back(currentValue);
					break;
				case ']':
					currentValue = valueStack.back();
					valueStack.pop_back();
					break;
			}
		}
	}

private:
	std::string CurrentString = "X";
	float lineLength = 0.25f;
	float adjustAngle = 25.0f;

	const glm::vec2& getLineEndPos(glm::vec2 startPos, float angle)
	{
		return glm::vec2(startPos.x + (lineLength * std::cos(angle)), startPos.y + (lineLength * std::sin(angle)));
	}
};