#pragma once

#include <glad/glad.h>
#include <iostream>
#include "stb_image.h"

class Texture
{
public:
	unsigned int ID;
	int Width, Height, Channels;

	Texture(const char* filePath)
	{
		glGenTextures(1, &ID);
		glBindTexture(GL_TEXTURE_2D, ID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT); // wrapping
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // minification/magnification filtering
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		unsigned char* data = stbi_load(filePath, &Width, &Height, &Channels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, data); // Make texture using image data
			glGenerateMipmap(GL_TEXTURE_2D); // Generate them mip maps
		}
		else
		{
			std::cout << "Failed to load texture: " + std::string(filePath) << std::endl;
		}

		glBindTexture(GL_TEXTURE_2D, 0); // Unbind texture

		stbi_image_free(data); // free loaded memory
	}

private:
};