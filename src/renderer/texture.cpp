#include "renderer.h"
#include <iostream>



#include <vector>


namespace
{
	inline unsigned int get_next_power_of_two(const unsigned int& number)
	{
		unsigned int powertwonumber{ number };
		--powertwonumber;
		powertwonumber |= powertwonumber >> 1;
		powertwonumber |= powertwonumber >> 2;
		powertwonumber |= powertwonumber >> 4;
		powertwonumber |= powertwonumber >> 8;
		powertwonumber |= powertwonumber >> 16;
		++powertwonumber;
		return powertwonumber;
	}

	void create_chess_board_image_rgba(std::vector<unsigned char>& image_out, unsigned int& width, unsigned int& height)
	{
		const int checkImageWidth = 64;
		const int checkImageHeight = 64;
		
		image_out.resize(checkImageWidth*checkImageHeight * 4);
		auto* checkImage = image_out.data();

		for (auto i = 0; i < checkImageHeight; ++i) {
			for (auto j = 0; j < checkImageWidth; ++j) {
				const auto ii = i / 8;
				const auto jj = j / 8;
				const auto c = (ii + jj) % 2 * 255;
				
				checkImage[i*(checkImageWidth*4) + j * 4 + 0] = (GLubyte)c;
				checkImage[i*(checkImageWidth*4) + j * 4 + 1] = (GLubyte)c;
				checkImage[i*(checkImageWidth*4) + j * 4 + 2] = (GLubyte)c;
				checkImage[i*(checkImageWidth*4) + j * 4 + 3] = (GLubyte)255;
			}
		}

		width = checkImageWidth;
		height = checkImageHeight;
	}	
};



unsigned int renderer::texture::load_png_rgba(const char* filename)
{
	// Load file and decode image.
	std::vector<unsigned char> image;
	unsigned int width, height;

	// try to load the image. if we fail to do so, initialize the default checkboard image
	if (strlen(filename) == 0 || lodepng::decode(image, width, height, filename) != 0) create_chess_board_image_rgba(image, width, height);

	// check if the loaded image is a power of two
	if (!(width && !(width & (width - 1))) ||
		!(height && !(height & (height - 1)))) return -1;

	unsigned int gl_texture_id;

	glGenTextures(1, &gl_texture_id);// then we need to tell OpenGL that we are generating a texture
	glBindTexture(GL_TEXTURE_2D, gl_texture_id); // now we bind the texture that we are working with
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); // set texture environment properties

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.data());

	return gl_texture_id;
}