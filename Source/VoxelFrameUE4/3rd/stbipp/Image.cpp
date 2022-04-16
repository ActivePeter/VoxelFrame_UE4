#include "stbipp/Image.hpp"

#include <exception>
#include "stdexcept"

namespace stbipp
{
	Image::Image(int width, int height)
	{
		resizeData(width, height);
	}

	Image::Image(int width, int height, const Color4f& color) : Image(width, height)
	{
		fill(color);
	}

	Image::Image(void* data, int width, int height, ImageFormat pixelFormat) : Image(width, height)
	{
		if (isFormat8Bits(pixelFormat))
		{
			auto* ucdata = static_cast<unsigned char*>(data);
			copyData(ucdata, width, height, pixelFormat);
		}
		else if (isFormat16Bits(pixelFormat))
		{
			auto* usdata = static_cast<unsigned short*>(data);
			copyData(usdata, width, height, pixelFormat);
		}
		else if (isFormat32Bits(pixelFormat))
		{
			auto* fdata = static_cast<float*>(data);
			copyData(fdata, width, height, pixelFormat);
		}
	}

	Image::Image(const Image& other) : Image(other.m_width, other.m_height)
	{
		copyData(other);
	}

	Image::Image(Image&& other) : m_width(other.width()), m_height(other.height())
	{
		std::swap(m_data, other.m_data);
	}

	const Image::Color* Image::data() const
	{
		return m_data.data();
	}

	void Image::fill(const Color4f& color)
	{
		std::fill(m_data.begin(), m_data.end(), color);
	}

	void Image::resize(int width, int height)
	{
		std::vector<Color> newData(static_cast<std::size_t>(width) * static_cast<std::size_t>(height));
		const auto minWidth = std::min(width, m_width);
		const auto minHeight = std::min(height, m_height);
		for (int rowIndex = 0; rowIndex < minHeight; ++rowIndex)
		{
			for (int columnIndex = 0; columnIndex < minWidth; ++columnIndex)
			{
				std::size_t index = static_cast<std::size_t>(rowIndex) * static_cast<std::size_t>(width) +
					static_cast<std::size_t>(columnIndex);
				newData[index] = (*this)(columnIndex, rowIndex);
			}
		}
		std::swap(m_data, newData);
		m_width = width;
		m_height = height;
	}

	int Image::height() const
	{
		return m_height;
	}

	int Image::width() const
	{
		return m_width;
	}

	Image::Color Image::operator()(int column, int row) const
	{
		if (column > m_width || column < 0 || row < 0 || row > m_height)
		{
			throw std::runtime_error("Trying to access out of range value");
		}
		return m_data[row * m_width + column];
	}

	Image::Color& Image::operator()(int column, int row)
	{
		if (column > m_width || column < 0 || row < 0 || row > m_height)
		{
			throw std::runtime_error("Trying to access out of range value");
		}
		return m_data[row * m_width + column];
	}

	Image& Image::operator=(const Image& other)
	{
		resizeData(other.width(), other.height());
		copyData(other);
		return *this;
	}

	void Image::copyData(const Image& other)
	{
		for (int rowIndex = 0; rowIndex < other.m_height; ++rowIndex)
		{
			for (int columnIndex = 0; columnIndex < other.m_width; ++columnIndex)
			{
				(*this)(columnIndex, rowIndex) = static_cast<Color>(other(columnIndex, rowIndex));
			}
		}
	}

	void Image::copyDataFrom(const Image& other, int x, int y)
	{
		int write_w = this->width() - x;
		int write_h = this->height() - y;
		write_w = std::min(write_w, other.width());
		write_h = std::min(write_h, other.height());

		for (int rowIndex = 0; rowIndex < write_h/*other.m_height*/; ++rowIndex)
		{
			for (int columnIndex = 0; columnIndex < write_w/*other.m_width*/; ++columnIndex)
			{
				(*this)(columnIndex + x, rowIndex + y) = static_cast<Color>(other(columnIndex, rowIndex));
			}
		}
	}

	void Image::copyData(unsigned char* data, int width, int height, ImageFormat pixelFormat)
	{
		for (int rowIndex = 0; rowIndex < height; ++rowIndex)
		{
			for (int columnIndex = 0; columnIndex < width; ++columnIndex)
			{
				Color4uc color{};
				auto channels = formatChannelCount(pixelFormat);
				for (int colorComponent = 0; colorComponent < channels; ++colorComponent)
				{
					color[colorComponent] =
						*(data + ((width * channels * rowIndex) + columnIndex * channels + colorComponent));
				}
				(*this)(columnIndex, rowIndex) = color;
			}
		}
	}

	void Image::copyData(unsigned short* data, int width, int height, ImageFormat pixelFormat)
	{
		for (int rowIndex = 0; rowIndex < height; ++rowIndex)
		{
			for (int columnIndex = 0; columnIndex < width; ++columnIndex)
			{
				Color4us color{};
				auto channels = formatChannelCount(pixelFormat);
				for (int colorComponent = 0; colorComponent < channels; ++colorComponent)
				{
					color[colorComponent] =
						*(data + ((width * channels * rowIndex) + columnIndex * channels + colorComponent));
				}
				(*this)(columnIndex, rowIndex) = color;
			}
		}
	}

	void Image::copyData(float* data, int width, int height, ImageFormat pixelFormat)
	{
		for (int rowIndex = 0; rowIndex < height; ++rowIndex)
		{
			for (int columnIndex = 0; columnIndex < width; ++columnIndex)
			{
				Color4f color{};
				auto channels = formatChannelCount(pixelFormat);
				for (int colorComponent = 0; colorComponent < channels; ++colorComponent)
				{
					color[colorComponent] =
						*(data + ((width * channels * rowIndex) + columnIndex * channels + colorComponent));
				}
				(*this)(columnIndex, rowIndex) = color;
			}
		}
	}

	void Image::resizeData(int width, int height)
	{
		if (width < 0 || height < 0)
		{
			throw std::invalid_argument("New image dimensions must be positive integers!");
		}
		m_height = height;
		m_width = width;
		m_data.resize(height * width);
	}

} // namespace stbipp
