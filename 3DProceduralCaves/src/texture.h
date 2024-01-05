#pragma once



class Texture
{
public:
	unsigned int ID;
	int Width, Height, Channels;

	Texture(const char* filePath);

private:
};