#include "cave_generator.h"

#include <vector>
#include <unordered_map>

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
	// Should only seed on full regenerate
	// std::srand(randomSeed);

	std::string nextString = "";
	for (int i = 0; i < CurrentString.length(); i++)
	{
		switch (CurrentString[i])
		{
			case 'X':
				nextString += "FF[-FX][+FX]";
				break;
			case 'F':
			{
				float chance = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
				nextString += chance > 0.75f ? "FFFF" : chance > 0.5f ? "FF" : "F";
				break;
			}
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

	std::srand(randomSeed);

	LSystemValue currentValue = LSystemValue{ glm::vec2(0.0f, 0.0f), 0.0f, 0.0f };
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
					currentValue.AdjustAngle = 35.0f + (std::rand() % (int)(20.0f));

					TunnelIntersectionMesh* tunnelPiece = new TunnelIntersectionMesh(currentValue.AdjustAngle);
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
				currentValue.Rotation -= currentValue.AdjustAngle;
				break;
			case '+':
				currentValue.Rotation += currentValue.AdjustAngle;
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
	for (TunnelMesh* mesh : tunnelMeshes)
		mesh->splitMeshTriangles(preBlurSplitting);	
	
	if (proceduralStage >= 3)
	{
		for (int i = 0; i < 2; i++)
		{
			for (TunnelMesh* mesh : tunnelMeshes)
				mesh->generateGeometryBlurring(tunnelMeshes);

			for (TunnelMesh* mesh : tunnelMeshes)
				mesh->pushGeometryBlurring();
		}
	}

	for (TunnelMesh* mesh : tunnelMeshes)
		mesh->splitMeshTriangles(postBlurSplitting);

	if (proceduralStage >= 4)
	{
		for (TunnelMesh* mesh : tunnelMeshes)
			mesh->generatePerlinNoise();
	}

	calculateMeshNormals();

	for (TunnelMesh* mesh : tunnelMeshes)
		mesh->generate();
}

const glm::vec2& CaveGenerator::getLineEndPos(glm::vec2 startPos, float radians)
{
	return glm::vec2(startPos.x + (lineLength * std::cos(radians)), startPos.y + (lineLength * std::sin(radians)));
}

void CaveGenerator::calculateMeshNormals()
{
	std::unordered_map<glm::vec3, glm::vec3, Vec3KeyFuncs, Vec3KeyFuncs> vertexNormals; // stores each unique vertex

	for (TunnelMesh* mesh : tunnelMeshes)
	{
		mesh->calculateNormals();

		const glm::mat4 transformMat = mesh->getWorldMatrix();
		for (const Vertex& vertex : mesh->getMesh().getVertices())
		{
			const glm::vec3 vertexWorldPos = mesh->transformVecByMatrix(vertex.Position, transformMat);
			vertexNormals[vertexWorldPos] += vertex.Normal;
		}
	}

	// normalize normals
	//drawLines.clear();
	for (auto const& value : vertexNormals)
	{
		vertexNormals[value.first] = glm::normalize(value.second);

		//drawLines.push_back(new Line(value.first, value.first + value.second * 0.1f));
	}

	std::cout << vertexNormals.size() << std::endl;

	// update mesh normals
	for (TunnelMesh* mesh : tunnelMeshes)
	{
		std::vector<Vertex>& meshVertices = mesh->getMesh().getVertices();

		const glm::mat4 transformMat = mesh->getWorldMatrix();
		for (int i = 0; i < meshVertices.size(); i++)
		{
			const glm::vec3 vertexWorldPos = mesh->transformVecByMatrix(meshVertices[i].Position, transformMat);
			meshVertices[i].Normal = vertexNormals[vertexWorldPos];
		}

		// create normal lines
		mesh->createNormalLines();
	}
}