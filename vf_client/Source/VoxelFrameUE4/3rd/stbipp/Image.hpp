#pragma once

#include "StbippSymbols.h"
#include "stbipp/Color.hpp"
#include "stbipp/ImageFormat.hpp"

#include <memory>
#include <vector>

namespace stbipp
{
	/**
	 * @brief The Image class is a 2D pixel matrix
	 * A pixel with 4 float channels
	 */
	class STBIPP_API Image
	{
	public:
		using Color = Color4f;

		/**
		 * @brief Default image constructor
		 */
		explicit Image() = default;

		/**
		 * @brief Image contructor, resize the image with the given dimensions
		 * @param[in] width The image width
		 * @param[in] height The image height
		 */
		Image(int width, int height);

		/**
		 * @brief Image contructor, filling the content with the given color
		 * @param[in] width The image width
		 * @param[in] height The image height
		 * @param[in] color The image will be filled with this color
		 */
		Image(int width, int height, const Color4f& color);

		/**
		 * @brief Image constructor
		 * @param[in] data The data that will populate the pixel matrix
		 * @param[in] width Image width
		 * @param[in] height Image height
		 * @param[in] pixelFormat The data pixel format
		 * (must be set depending on the data argument
		 * e.g : RGBA8 this means that data is pointing to an unsigned char array of dimension :
		 * width * height * formatChannelCount(pixelFormat))
		 */
		Image(void* data, int width, int height, ImageFormat pixelFormat);

		/**
		 * @brief Image copy constructor
		 * @param[in] other The image to copy
		 */
		Image(const Image& other);

		/**
		 * @brief Image move constructor
		 * @param[in] other The image moved
		 */
		Image(Image&& other);

		/**
		 * @brief Image destructor
		 */
		~Image() = default;

		/**
		 * @brief Access the data of the first element
		 * @return Pointer to the color matrix data
		 */
		const Color* data() const;

		/**
		 * @brief Fill the image with the given color
		 * @param[in] color The image will be filled with this color
		 */
		void fill(const Color4f& color);

		/**
		 * @brief Resize the image with the given dimensions
		 * @param[in] width The new image width
		 * @param[in] height The new image height
		 */
		void resize(int width, int height);

		/**
		 * @brief Cast data to another color type
		 * @tparam ColorType The new color type (e.g : Color3uc, Colorus,...)
		 * @return The pixel matrix casted
		 */
		template<class ColorType>
		std::vector<ColorType> castData() const
		{
			std::size_t size = m_width * m_height;
			std::vector<ColorType> castedValue(size);
			for (std::size_t index = 0; index < size; ++index)
			{
				castedValue[index] = m_data[index];
			}
			return castedValue;
		}

		/**
		 * @brief Image height getter
		 * @return The image height
		 */
		int height() const;

		/**
		 * @brief Image width getter
		 * @return The image width
		 */
		int width() const;

		/**
		 * @brief Accessor to the color at the specified coordinate
		 * @param[in] column The x coordinate
		 * @param[in] row The y coordinate
		 * @return The color at the given coordinate
		 */
		Color operator()(int column, int row) const;

		/**
		 * @brief Accessor to the color at the specified coordinate
		 * @param[in] column The x coordinate
		 * @param[in] row The y coordinate
		 * @return The color at the given coordinate
		 */
		Color& operator()(int column, int row);

		/**
		 * @brief Copy operator
		 * @param[in] other The image to copy
		 * @return A reference to the image
		 */
		Image& operator=(const Image& other);

		/**
		 * @brief Copy the data of the other image into this
		 * @param[in] other The image to copy
		 */
		void copyData(const Image& other);

		void copyDataFrom(const Image& other, int x, int y);

		/**
		 * @brief Copy the data at the given location
		 * @param[in] data Pointer to the data
		 * @param[in] width Image width
		 * @param[in] height Image height
		 * @param[in] pixelFormat The pixel format describing the data format
		 */
		void copyData(unsigned char* data, int width, int height, ImageFormat pixelFormat);

		/**
		 * @brief Copy the data at the given location
		 * @param[in] data Pointer to the data
		 * @param[in] width Image width
		 * @param[in] height Image height
		 * @param[in] pixelFormat The pixel format describing the data format
		 */
		void copyData(unsigned short* data, int width, int height, ImageFormat pixelFormat);

		/**
		 * @brief Copy the data at the given location
		 * @param[in] data Pointer to the data
		 * @param[in] width Image width
		 * @param[in] height Image height
		 * @param[in] pixelFormat The pixel format describing the data format
		 */
		void copyData(float* data, int width, int height, ImageFormat pixelFormat);

		/**
		 * @brief Resize the pixel matrix
		 * @param[in] width New image width
		 * @param[in] height New image height
		 */
		void resizeData(int width, int height);
		std::vector<Color> m_data;

	private:
		int m_width;
		int m_height;
	};

} // namespace stbipp
