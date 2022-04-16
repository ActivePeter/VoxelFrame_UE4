#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stbipp/ImageExporter.hpp"

#include <algorithm>
#include <functional>
#include <stb_image_write.h>
#include <unordered_set>

namespace
{
using SaveFunction = std::function<bool(char const*, int, int, int, const void*)>;

const std::unordered_set<std::string> getOneByteFileSavedFormat();
const std::unordered_set<std::string> getSupportedSaveFileFormat();

// 8
bool write_png(char const* filename, int w, int h, int comp, const void* data)
{
    return stbi_write_png(filename, w, h, comp, data, w * comp);
}
// 8
bool write_bmp(char const* filename, int w, int h, int comp, const void* data)
{
    return stbi_write_bmp(filename, w, h, comp, data);
}
// 8
bool write_tga(char const* filename, int w, int h, int comp, const void* data)
{
    return stbi_write_tga(filename, w, h, comp, data);
}
// 32
bool write_hdr(char const* filename, int w, int h, int comp, const void* data)
{
    return stbi_write_hdr(filename, w, h, comp, static_cast<const float*>(data));
}

bool write_jpg(char const* filename, int w, int h, int comp, const void* data)
{
    return stbi_write_jpg(filename, w, h, comp, data, 100);
}

bool isOneByteFileSavedFormat(const std::string& extension)
{
    return getOneByteFileSavedFormat().count(extension);
}

bool isFileSavedFormatSupported(const std::string& extension)
{
    return getSupportedSaveFileFormat().count(extension);
}

const std::unordered_set<std::string> getOneByteFileSavedFormat()
{
    return {"png", "bmp", "tga", "jpg", "jpeg"};
}

const std::unordered_set<std::string> getSupportedSaveFileFormat()
{
    auto fileFormat = getOneByteFileSavedFormat();
    fileFormat.insert({"hdr"});
    return fileFormat;
}

} // namespace

namespace stbipp
{
bool saveImage(const std::string& path, const Image& image, const ImageSaveFormat pixelFormat)
{
    std::string pathExtension = path.substr(path.find_last_of(".") + 1);

    if(pathExtension.empty())
    {
        return false;
    }

    std::transform(pathExtension.begin(), pathExtension.end(), pathExtension.begin(), [](unsigned char c) {
        return std::tolower(c);
    });

    if(!isFileSavedFormatSupported(pathExtension))
    {
        return false;
    }

    const int channels = formatChannelCount(pixelFormat);

    ::SaveFunction function;

    if(pathExtension == "png")
    {
        function = write_png;
    }
    else if(pathExtension == "bmp")
    {
        function = write_bmp;
    }
    else if(pathExtension == "tga")
    {
        function = write_tga;
    }
    else if(pathExtension == "jpg" || pathExtension == "jpeg")
    {
        function = write_jpg;
    }

    else if(pathExtension == "hdr")
    {
        function = write_hdr;
    }

    if(isOneByteFileSavedFormat(pathExtension))
    {
        if(pixelFormat == ImageSaveFormat::LUM)
        {
            const auto dataVector = image.castData<Coloruc>();
            return function(path.data(), image.width(), image.height(), channels, dataVector.data());
        }
        else if(pixelFormat == ImageSaveFormat::LUMA)
        {
            const auto dataVector = image.castData<Color2uc>();
            return function(path.data(), image.width(), image.height(), channels, dataVector.data());
        }
        else if(pixelFormat == ImageSaveFormat::RGB)
        {
            const auto dataVector = image.castData<Color3uc>();
            return function(path.data(), image.width(), image.height(), channels, dataVector.data());
        }
        else if(pixelFormat == ImageSaveFormat::RGBA)
        {
            const auto dataVector = image.castData<Color4uc>();
            return function(path.data(), image.width(), image.height(), channels, dataVector.data());
        }
    }

    else
    {
        if(pixelFormat == ImageSaveFormat::LUM)
        {
            auto dataVector = image.castData<Colorf>();
            return function(path.data(), image.width(), image.height(), channels, dataVector.data());
        }
        else if(pixelFormat == ImageSaveFormat::LUMA)
        {
            auto dataVector = image.castData<Color2f>();
            return function(path.data(), image.width(), image.height(), channels, dataVector.data());
        }
        else if(pixelFormat == ImageSaveFormat::RGB)
        {
            auto dataVector = image.castData<Color3f>();
            return function(path.data(), image.width(), image.height(), channels, dataVector.data());
        }
        else if(pixelFormat == ImageSaveFormat::RGBA)
        {
            auto dataVector = image.castData<Color4f>();
            return function(path.data(), image.width(), image.height(), channels, dataVector.data());
        }
    }
    return false;
}

int formatChannelCount(const ImageSaveFormat& format)
{
    switch(format)
    {
        case ImageSaveFormat::LUM: return 1;
        case ImageSaveFormat::LUMA: return 2;
        case ImageSaveFormat::RGB: return 3;
        case ImageSaveFormat::RGBA: return 4;
    }
    return -1;
}

} // namespace stbipp
