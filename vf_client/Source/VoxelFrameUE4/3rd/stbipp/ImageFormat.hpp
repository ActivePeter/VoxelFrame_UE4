#pragma once

#include "StbippSymbols.h"

namespace stbipp
{
/**
 * @brief The ImageFormat used
 */
enum class ImageFormat
{

    LUM8,  /// Unsigned char luminance
    LUM16, /// Unsigned short luminance
    LUM32, /// Float luminance

    LUMA8,  /// Unsigned char luminance with alpha
    LUMA16, /// Unsigned short luminance with alpha
    LUMA32, /// Float luminance with alpha

    RGB8,  /// Unsigned char RGB
    RGB16, /// Unsigned short RGB
    RGB32, /// Float RGB

    RGBA8,  /// Unsigned char RGBA
    RGBA16, /// Unsigned short RGBA
    RGBA32, /// Float RGBA

    UNDEFINED = -1
};

/**
 * @brief Is a format encoded with 8 bits per channel
 * @param[in] format The format to test
 * @return true if a channel is encoded with 8 bits
 */
STBIPP_API bool isFormat8Bits(const ImageFormat& format);

/**
 * @brief Is a format encoded with 16 bits per channel
 * @param[in] format The format to test
 * @return true if a channel is encoded with 16 bits
 */
STBIPP_API bool isFormat16Bits(const ImageFormat& format);

/**
 * @brief Is a format encoded with 32 bits per channel
 * @param[in] format The format to test
 * @return true if a channel is encoded with 32 bits
 */
STBIPP_API bool isFormat32Bits(const ImageFormat& format);

/**
 * @brief Return the numbers of channel the given format have
 * @param[in] format The format to test
 * @return The number of channel in the given format
 */
STBIPP_API int formatChannelCount(const ImageFormat& format);

} // namespace stbipp
