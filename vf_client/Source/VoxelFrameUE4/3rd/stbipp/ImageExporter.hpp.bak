#pragma once

#include "stbipp/Image.hpp"

#include <string>

namespace stbipp
{
/**
 * @brief The format to use during save operation
 */
enum class ImageSaveFormat
{
    LUM,  /// Save grayscale only image
    LUMA, /// Save grayscale with alpha image
    RGB,  /// Save the image as an RGB image
    RGBA  /// Save the image as an RGB with alpha image
};

/**
 * @brief Save the given image at the given path with the specified format
 *  * Stbipp uses the same save function you'll find in stb_image_write meaning that you are able to load the same file
 * format :
 * - JPEG
 * - PNG
 * - BMP
 * - TGA
 * - HDR
 * @param[in] path Path to the image to save
 * @param[in] image The image containing the data to save
 * @param[in] pixelFormat The pixel format to use
 * @return true if the save operation was successful
 */
STBIPP_API bool saveImage(const std::string& path, const Image& image, const ImageSaveFormat pixelFormat);

/**
 * @brief Return the numbers of channel the given format have
 * @param[in] format The format to test
 * @return The number of channel in the given format
 */
STBIPP_API int formatChannelCount(const ImageSaveFormat& format);

} // namespace stbipp
