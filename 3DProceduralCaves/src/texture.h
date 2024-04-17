#pragma once



class Texture
{
public:
	unsigned int ID;
	int Width, Height, Channels;
	bool HasLoaded = false;

	Texture(const char* filePath);

private:
};