#pragma once

#include <vector>
#include "line.h"

class CaveGenerator2
{
public:
	std::vector<Line*> drawLines;
	int currentIteration = 0;

	CaveGenerator2()
	{

	}

	~CaveGenerator2()
	{
		for (Line* line : drawLines)
			delete line;
	}

	void Generate()
	{
		std::string nextString = "";
		for (int i = 0; i < CurrentString.length(); i++)
		{
			// rules
			switch (CurrentString[i])
			{
			case '1':
				nextString += "11";
				break;
			case '0':
				nextString += "1[0]0";
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
			// draw rules
			switch (CurrentString[i])
			{
			case '1':
				glm::vec2 endPos = getLineEndPos(currentValue.Position, currentValue.Rotation);

				// create new line mesh, swap x/y around and invert z so tree is facing forward
				drawLines.push_back(new Line(glm::vec3(currentValue.Position.y, 0.0f, currentValue.Position.x), glm::vec3(endPos.y, 0.0f, endPos.x)));

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
				currentValue.Rotation -= adjustAngle;
				break;
			case ']':
				currentValue = valueStack.back();
				valueStack.pop_back();
				currentValue.Rotation += adjustAngle;
				break;
			}
		}
	}

private:
	std::string CurrentString = "0";
	float lineLength = 1.0f;
	float adjustAngle = 45.0f;

	const glm::vec2& getLineEndPos(glm::vec2 startPos, float angle)
	{
		return glm::vec2(startPos.x + (lineLength * std::cos(angle)), startPos.y + (lineLength * std::sin(angle)));
	}
};