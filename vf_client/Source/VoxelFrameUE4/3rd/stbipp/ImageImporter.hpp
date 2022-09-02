#pragma once
#include "StbippSymbols.h"
#include "stbipp/Image.hpp"

#include <string>

namespace stbipp
{
	unsigned char* loadUCharImage(const std::string& path, int& width, int& height, const stbipp::ImageFormat& format);
	void freeStbData(void* data);
	/**
	 * @brief Load an image at the given path with the given pixel format
	 * Stbipp uses the same load function you'll find in stb_image meaning that you are able to load the same file format :
	 * - JPEG
	 * - PNG
	 * - BMP
	 * - PSD
	 * - TGA
	 * - GIF
	 * - HDR
	 * - PIC
	 * - PNM (.ppm and .pgm)
	 * But it also contains the same limitations that you can find at :
	 * https://github.com/nothings/stb/blob/master/stb_image.h
	 * @param[in] path Path to the image to load
	 * @param[out] image The image which will contains the data (all contained data will be erased)
	 * @param[in] pixelFormat The pixel format to use
	 * @return true if the loading was successful
	 */
	STBIPP_API bool loadImage(const std::string& path, Image& image, const ImageFormat pixelFormat);
} // namespace stbipp
