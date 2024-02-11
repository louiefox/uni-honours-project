#include "cave_generator.h"

#include <vector>

#include "line.h"
#include "tunnel_mesh.h"
#include "tunnel_intersection_mesh.h"

CaveGenerator::CaveGenerator()
{

}

CaveGenerator::~CaveGenerator()
{
	for (Line* line : drawLines)
		delete line;

	for (TunnelMesh* mesh : tunnelMeshes)
		delete mesh;
}

void CaveGenerator::GenerateNext()
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

void CaveGenerator::ReGenerateCurrent()
{
	std::cout << "Iteration " << currentIteration << std::endl;
	std::cout << CurrentString << std::endl;

	UpdateDraw();
}

void CaveGenerator::UpdateDraw()
{
	for (TunnelMesh* mesh : tunnelMeshes)
		delete mesh;

	tunnelMeshes = {};

	for (Line* line : drawLines)
		delete line;

	drawLines = {};

	LSystemValue currentValue = LSystemValue{ glm::vec2(0.0f, 0.0f), 0.0f };
	std::vector<LSystemValue> valueStack;
	for (int i = 0; i < CurrentString.length(); i++)
	{
		char previousCharacter = i > 0 ? CurrentString[i - 1] : ' ';
		char previousPreviousCharacter = i > 1 ? CurrentString[i - 2] : ' ';
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

					if (previousCharacter == 'F')
					{
						TunnelMesh* previousPiece = tunnelMeshes[tunnelMeshes.size() - 1];
						tunnelPiece->setPreviousTunnelMesh(previousPiece);
						previousPiece->setNextTunnelMesh(tunnelPiece);
					}

					tunnelMeshes.push_back(tunnelPiece);
					tunnelIntersectionMeshes.push_back(tunnelPiece); // Add to intersection meshes for lookup
				}
				else if (previousCharacter != '-' && previousCharacter != '+') // place normal tunnel if not intersection
				{
					TunnelMesh* tunnelPiece = new TunnelMesh();
					tunnelPiece->SetPosition(glm::vec3(currentValue.Position.y + (LINE_END_POS.y - currentValue.Position.y) / 2, 0.0f, -currentValue.Position.x + (-LINE_END_POS.x - -currentValue.Position.x) / 2));
					tunnelPiece->SetRotation(glm::vec3(0.0, -currentValue.Rotation, 0.0));

					if (previousCharacter == 'F' && (previousPreviousCharacter == '-' || previousPreviousCharacter == '+')) // Intersection previously
					{
						TunnelMesh* previousPiece = tunnelIntersectionMeshes[0];

						if (previousPreviousCharacter == '-')
						{
							previousPiece->setNextTunnelMesh(tunnelPiece);
						}					
						else //if (previousPreviousCharacter == '+')
						{
							previousPiece->setNextTunnelMesh2(tunnelPiece);
						}

						tunnelPiece->setPreviousTunnelMesh(previousPiece);
					}
					else if (previousCharacter == 'F')
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

	// Generate and blur meshes
	//for (int i = 0; i < 1; i++)
	//{
	//	for (TunnelMesh* mesh : tunnelMeshes)
	//		mesh->generateGeometryBlurring();

	//	for (TunnelMesh* mesh : tunnelMeshes)
	//		mesh->pushGeometryBlurring();
	//}

	if (tempDevEnablePerlin)
	{
		for (TunnelMesh* mesh : tunnelMeshes)
			mesh->generatePerlinNoise();
	}

	for (TunnelMesh* mesh : tunnelMeshes)
		mesh->generate();
}

void CaveGenerator::SetAdjustAngle(float newAdjustAngle)
{
	adjustAngle = newAdjustAngle;
}

const glm::vec2& CaveGenerator::getLineEndPos(glm::vec2 startPos, float radians)
{
	return glm::vec2(startPos.x + (lineLength * std::cos(radians)), startPos.y + (lineLength * std::sin(radians)));
}