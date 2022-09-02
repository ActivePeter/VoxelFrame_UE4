
#include "stbipp/ImageFormat.hpp"

namespace stbipp
{
bool isFormat8Bits(const ImageFormat& format)
{
    if(format == ImageFormat::LUM8 || format == ImageFormat::LUMA8 || format == ImageFormat::RGB8 ||
       format == ImageFormat::RGBA8)
    {
        return true;
    }

    return false;
}

bool isFormat16Bits(const ImageFormat& format)
{
    if(format == ImageFormat::LUM16 || format == ImageFormat::LUMA16 || format == ImageFormat::RGB16 ||
       format == ImageFormat::RGBA16)
    {
        return true;
    }

    return false;
}

bool isFormat32Bits(const ImageFormat& format)
{
    if(format == ImageFormat::LUM32 || format == ImageFormat::LUMA32 || format == ImageFormat::RGB32 ||
       format == ImageFormat::RGBA32)
    {
        return true;
    }

    return false;
}

int formatChannelCount(const ImageFormat& format)
{
    switch(format)
    {
        case ImageFormat::LUM8:
        case ImageFormat::LUM16:
        case ImageFormat::LUM32: return 1;
        case ImageFormat::LUMA8:
        case ImageFormat::LUMA16:
        case ImageFormat::LUMA32: return 2;
        case ImageFormat::RGB8:
        case ImageFormat::RGB16:
        case ImageFormat::RGB32: return 3;
        case ImageFormat::RGBA8:
        case ImageFormat::RGBA16:
        case ImageFormat::RGBA32: return 4;
        default: return -1;
    }
}

} // namespace stbipp
