#pragma once

#include <vector>

#include "line.h"
#include "tunnel_mesh.h"
#include "tunnel_intersection_mesh.h"

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
	std::vector<TunnelIntersectionMesh*> tunnelIntersectionMeshes;
	int currentIteration = 0;
	bool tempDevEnablePerlin = false;

	CaveGenerator()
	{

	}

	~CaveGenerator()
	{
		for (Line* line : drawLines)
			delete line;		
		
		for (TunnelMesh* mesh : tunnelMeshes)
			delete mesh;		
		
		for (TunnelIntersectionMesh* mesh : tunnelIntersectionMeshes)
			delete mesh;
	}

	void GenerateNext()
	{
		std::string nextString = "";
		for (int i = 0; i < CurrentString.length(); i++)
		{
			switch (CurrentString[i])
			{
				case 'X':
					nextString += "FF[-FX][+FX]";
					//nextString += "F+[[X]-X]-F[-FX]+X";
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
	
	void ReGenerateCurrent()
	{
		std::cout << "Iteration " << currentIteration << std::endl;
		std::cout << CurrentString << std::endl;

		UpdateDraw();
	}

	void UpdateDraw()
	{
		for (TunnelMesh* mesh : tunnelMeshes)
			delete mesh;

		tunnelMeshes = {};		
		
		for (TunnelIntersectionMesh* mesh : tunnelIntersectionMeshes)
			delete mesh;

		tunnelIntersectionMeshes = {};
		
		for (Line* line : drawLines)
			delete line;

		drawLines = {};

		LSystemValue currentValue = LSystemValue{ glm::vec2(0.0f, 0.0f), 0.0f };
		std::vector<LSystemValue> valueStack;
		for (int i = 0; i < CurrentString.length(); i++)
		{
			char previousCharacter = i > 0 ? CurrentString[i - 1] : ' ';
			char nextCharacter = (i + 1) < CurrentString.length() ? CurrentString[i + 1] : ' ';
			switch (CurrentString[i])
			{
				case 'F':
				{
					const glm::vec2 LINE_END_POS = getLineEndPos(currentValue.Position, glm::radians(currentValue.Rotation));

					// create new line mesh, swap x/y around and invert z so tree is facing forward
					drawLines.push_back(new Line(glm::vec3(currentValue.Position.y, 0.0f, -currentValue.Position.x), glm::vec3(LINE_END_POS.y, 0.0f, -LINE_END_POS.x)));

					// place tunnel building blocks
					if (nextCharacter == '[') // place intersection if start of branch
					{
						TunnelIntersectionMesh* tunnelPiece = new TunnelIntersectionMesh(adjustAngle);
						tunnelPiece->SetPosition(glm::vec3(LINE_END_POS.y, 0.0f, -LINE_END_POS.x));
						tunnelPiece->SetRotation(glm::vec3(0.0, -currentValue.Rotation, 0.0));

						tunnelIntersectionMeshes.push_back(tunnelPiece);
					}
					else if(previousCharacter != '-' && previousCharacter != '+') // place normal tunnel if not intersection
					{
						TunnelMesh* tunnelPiece = new TunnelMesh(tempDevEnablePerlin);
						tunnelPiece->SetPosition(glm::vec3(currentValue.Position.y + (LINE_END_POS.y - currentValue.Position.y) / 2, 0.0f, -currentValue.Position.x + (-LINE_END_POS.x - -currentValue.Position.x) / 2));
						tunnelPiece->SetRotation(glm::vec3(0.0, -currentValue.Rotation, 0.0));

						if (previousCharacter == 'F')
						{
							TunnelMesh* previousPiece = tunnelMeshes[tunnelMeshes.size() - 1];
							tunnelPiece->setPreviousTunnelMesh(previousPiece);
							previousPiece->setNextTunnelMesh(tunnelPiece);
						}

						tunnelMeshes.push_back(tunnelPiece);
					}

					currentValue.Position = LINE_END_POS;
					break;
				}
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

		if (tunnelMeshes.size() > 1)
		{
			tunnelMeshes[1]->generate();
			tunnelMeshes[1]->printTest();
		}

		// Generate meshes
		for (TunnelMesh* mesh : tunnelMeshes)
			mesh->generate();
	}

	void SetAdjustAngle(float newAdjustAngle)
	{
		adjustAngle = newAdjustAngle;
	}

private:
	std::string CurrentString = "X";
	float lineLength = 1.0f;
	float adjustAngle = 45.0f;

	const glm::vec2& getLineEndPos(glm::vec2 startPos, float radians)
	{
		return glm::vec2(startPos.x + (lineLength * std::cos(radians)), startPos.y + (lineLength * std::sin(radians)));
	}
};