#pragma once


namespace renderer
{
	namespace texture
	{
		/** Returns a value acquired via glGenTextures */
		unsigned int load_png_rgba(const char* filename = "");
	}
} 