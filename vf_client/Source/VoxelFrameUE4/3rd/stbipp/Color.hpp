
#pragma once
#include <array>
#include <limits>
#include <type_traits>

namespace stbipp
{
	template<class DataType, unsigned int nbComponents>
	class Color
	{
	public:
		/**
		 * @brief Color default constructor
		 */
		Color() : Color(static_cast<DataType>(0.0)) {};

		template<int I = nbComponents, typename std::enable_if<!(I > 0)>::type* = nullptr>
		explicit Color(DataType value) = delete;
		/**
		 * @brief Color constructor
		 * @param[in] value Fill the color with specified value
		 */
		template<int I = nbComponents, typename std::enable_if<(I > 0)>::type* = nullptr>
		explicit Color(DataType value)
		{
			std::fill(m_data.begin(), m_data.end(), DataType(0));
		}

		template<int I = nbComponents, typename std::enable_if<!(I > 1)>::type* = nullptr>
		explicit Color(DataType r, DataType g) = delete;

		/**
		 * @brief Color constructor
		 * @param[in] r Red channel value
		 * @param[in] g Green channel value
		 */
		template<int I = nbComponents, typename std::enable_if<(I > 1)>::type* = nullptr>
		explicit Color(DataType r, DataType g) : m_data({ r, g })
		{
		}

		template<int I = nbComponents, typename std::enable_if<!(I > 2)>::type* = nullptr>
		explicit Color(DataType r, DataType g, DataType b) = delete;

		/**
		 * @brief Color constructor
		 * @param[in] r Red channel value
		 * @param[in] g Green channel value
		 * @param[in] b Blue channel value
		 */
		template<int I = nbComponents, typename std::enable_if<(I > 2)>::type* = nullptr>
		explicit Color(DataType r, DataType g, DataType b) : m_data({ r, g, b })
		{
		}

		template<int I = nbComponents, typename std::enable_if<!(I > 3)>::type* = nullptr>
		explicit Color(DataType r, DataType g, DataType b, DataType a) = delete;

		/**
		 * @brief Color constructor
		 * @param[in] r Red channel value
		 * @param[in] g Green channel value
		 * @param[in] b Blue channel value
		 * @param[in] a Alpha channel value
		 */
		template<int I = nbComponents, typename std::enable_if<(I > 3)>::type* = nullptr>
		explicit Color(DataType r, DataType g, DataType b, DataType a) : m_data({ r, g, b, a })
		{
		}

		template<int I = nbComponents, typename std::enable_if<!(I > 0)>::type* = nullptr>
		DataType r() const = delete;
		/**
		 * @brief Access the red channel data
		 * @return Red channel value
		 */
		template<int I = nbComponents, typename std::enable_if<(I > 0)>::type* = nullptr>
		DataType r() const
		{
			return m_data[0];
		}

		template<int I = nbComponents, typename std::enable_if<!(I > 0)>::type* = nullptr>
		DataType& r() = delete;
		/**
		 * @brief Access the red channel data
		 * @return Red channel value
		 */
		template<int I = nbComponents, typename std::enable_if<(I > 0)>::type* = nullptr>
		DataType& r()
		{
			return m_data[0];
		}

		template<int I = nbComponents, typename std::enable_if<!(I > 1)>::type* = nullptr>
		DataType g() const = delete;
		/**
		 * @brief Access the green channel data
		 * @return Green channel value
		 */
		template<int I = nbComponents, typename std::enable_if<(I > 1)>::type* = nullptr>
		DataType g() const
		{
			return m_data[1];
		}

		template<int I = nbComponents, typename std::enable_if<!(I > 1)>::type* = nullptr>
		DataType& g() = delete;
		/**
		 * @brief Access the green channel data
		 * @return Green channel value
		 */
		template<int I = nbComponents, typename std::enable_if<(I > 1)>::type* = nullptr>
		DataType& g()
		{
			return m_data[1];
		}

		template<int I = nbComponents, typename std::enable_if<!(I > 2)>::type* = nullptr>
		DataType b() const = delete;
		/**
		 * @brief Access the blue channel data
		 * @return Blue channel value
		 */
		template<int I = nbComponents, typename std::enable_if<(I > 2)>::type* = nullptr>
		DataType b() const
		{
			return m_data[2];
		}

		template<int I = nbComponents, typename std::enable_if<!(I > 2)>::type* = nullptr>
		DataType& b() = delete;
		/**
		 * @brief Access the blue channel data
		 * @return Blue channel value
		 */
		template<int I = nbComponents, typename std::enable_if<(I > 2)>::type* = nullptr>
		DataType& b()
		{
			return m_data[2];
		}

		template<int I = nbComponents, typename std::enable_if<!(I > 3)>::type* = nullptr>
		DataType a() const = delete;
		/**
		 * @brief Access the alpha channel data
		 * @return Alpha channel value
		 */
		template<int I = nbComponents, typename std::enable_if<(I > 3)>::type* = nullptr>
		DataType a() const
		{
			return m_data[3];
		}

		template<int I = nbComponents, typename std::enable_if<!(I > 3)>::type* = nullptr>
		DataType& a() = delete;
		/**
		 * @brief Access the alpha channel data
		 * @return Alpha channel value
		 */
		template<int I = nbComponents, typename std::enable_if<(I > 3)>::type* = nullptr>
		DataType& a()
		{
			return m_data[3];
		}

		/**
		 * @brief Retrieve the number of channels
		 * @return Channels count
		 */
		unsigned int size() const
		{
			return nbComponents;
		}

		/**
		 * @brief Access the data of the first element
		 * @return A pointer to the first color element
		 */
		const DataType* data() const
		{
			return m_data.data();
		}

		/**
		 * @brief Access the data of the first element
		 * @return A pointer to the first color element
		 */
		DataType* data()
		{
			return m_data.data();
		}

		/**
		 * @brief Access the data the given index
		 * @return The value at the given index
		 */
		DataType operator[](int index) const
		{
			return m_data[index];
		}

		/**
		 * @brief Access the data the given index
		 * @return The value at the given index
		 */
		DataType& operator[](int index)
		{
			return m_data[index];
		}

	private:
		/**
		 * @brief Copy data of another color into this, with other uses the same data type as this but with possibly a
		 * different length
		 * @param[in] other Color to copy
		 */
		template<typename ODataType, unsigned int oDataSize>
		void copy(const Color<ODataType, oDataSize>& other,
			typename std::enable_if<std::is_same<DataType, ODataType>::value>::type* = nullptr)
		{
			std::size_t minSize = std::min(oDataSize, nbComponents);
			for (std::size_t index = 0; index < minSize; ++index)
			{
				m_data[index] = other[static_cast<int>(index)];
			}
			for (std::size_t index = minSize; index < nbComponents; ++index)
			{
				m_data[index] = 0.0;
			}
		}

		/**
		 * @brief Copy data of another color into this, with other using an integer type and this a floating type
		 * @param[in] other Color to copy
		 */
		template<class ODataType, unsigned int oDataSize>
		void copy(
			const Color<ODataType, oDataSize>& other,
			typename std::enable_if<std::is_floating_point<DataType>::value && !std::is_floating_point<ODataType>::value &&
			!std::is_same<DataType, ODataType>::value>::type* = nullptr)
		{
			std::size_t minSize = std::min(oDataSize, nbComponents);
			for (std::size_t index = 0; index < minSize; ++index)
			{
				m_data[index] =
					static_cast<DataType>(other[static_cast<int>(index)]) / std::numeric_limits<ODataType>::max();
			}
			for (std::size_t index = minSize; index < nbComponents; ++index)
			{
				m_data[index] = 0.0;
			}
		}

		/**
		 * @brief Copy data of another color into this, with other using a floating type and this an integer type
		 * @param[in] other Color to copy
		 */
		template<class ODataType, unsigned int oDataSize>
		void copy(
			const Color<ODataType, oDataSize>& other,
			typename std::enable_if<std::is_floating_point<ODataType>::value && !std::is_floating_point<DataType>::value &&
			!std::is_same<DataType, ODataType>::value>::type* = nullptr)
		{
			std::size_t minSize = std::min(oDataSize, nbComponents);
			for (std::size_t index = 0; index < minSize; ++index)
			{
				m_data[index] =
					static_cast<DataType>(std::numeric_limits<DataType>::max() * other[static_cast<int>(index)]);
			}
			for (std::size_t index = minSize; index < nbComponents; ++index)
			{
				m_data[index] = static_cast<DataType>(0.0);
			}
		}

		/**
		 * @brief Copy data of another color into this, with this and other using different integer types
		 * @param[in] other Color to copy
		 */
		template<class ODataType, unsigned int oDataSize>
		void copy(
			const Color<ODataType, oDataSize>& other,
			typename std::enable_if<!(std::is_floating_point<ODataType>::value || std::is_floating_point<DataType>::value) &&
			!std::is_same<DataType, ODataType>::value>::type* = nullptr)
		{
			std::size_t minSize = std::min(oDataSize, nbComponents);
			for (std::size_t index = 0; index < minSize; ++index)
			{
				m_data[index] =
					static_cast<DataType>((static_cast<float>(other[index]) * std::numeric_limits<DataType>::max()) /
						std::numeric_limits<ODataType>::max());
			}
			for (std::size_t index = minSize; index < nbComponents; ++index)
			{
				m_data[index] = 0.0;
			}
		}

	public:
		/**
		 * @brief Assignment operator
		 * @param[in] other The color to copy
		 * @return A reference to this
		 */
		template<class ODataType, unsigned int oDataSize>
		Color& operator=(const Color<ODataType, oDataSize>& other)
		{
			copy(other);
			return *this;
		}

		/**
		 * @brief Color copy constructor
		 */
		template<unsigned int oDataSize>
		Color(const Color<DataType, oDataSize>& other)
		{
			copy<DataType, oDataSize>(other);
		}

		/**
		 * @brief Color copy constructor
		 */
		template<class ODataType, unsigned int oDataSize>
		Color(const Color<ODataType, oDataSize>& other)
		{
			copy<ODataType, oDataSize>(other);
		}

		/**
		 * @brief Add the content of other to this
		 * @param[in] other The color to add
		 * @return A reference to this
		 */
		template<class ODataType, unsigned int oDataSize>
		Color& operator+=(const Color<ODataType, oDataSize>& other)
		{
			static_assert(oDataSize == nbComponents && std::is_same<DataType, ODataType>::value,
				"Both colors must be of the same size and type");
			for (auto comp = 0; comp < nbComponents; ++comp)
			{
				m_data[comp] += other[comp];
			}
			return *this;
		}

		/**
		 * @brief Add the content of other to this as an immutable object
		 * @param[in] other The color to add
		 * @return The addition of this and other
		 */
		template<class ODataType, unsigned int oDataSize>
		Color operator+(const Color<ODataType, oDataSize>& other) const
		{
			static_assert(oDataSize == nbComponents && std::is_same<DataType, ODataType>::value,
				"Both colors must be of the same size and type");
			Color temp{ *this };
			return temp += other;
		}

		/**
		 * @brief Substract the content of this by other
		 * @param[in] other The color to substract
		 * @return A reference to this
		 */
		template<class ODataType, unsigned int oDataSize>
		Color& operator-=(const Color<ODataType, oDataSize>& other)
		{
			static_assert(oDataSize == nbComponents && std::is_same<DataType, ODataType>::value,
				"Both colors must be of the same size and type");
			for (auto comp = 0; comp < nbComponents; ++comp)
			{
				m_data[comp] -= other[comp];
			}
			return *this;
		}

		/**
		 * @brief Substract the content of this by other as an immutable object
		 * @param[in] other The color to substract
		 * @return The substraction of this by other
		 */
		template<class ODataType, unsigned int oDataSize>
		Color operator-(const Color<ODataType, oDataSize>& other) const
		{
			static_assert(oDataSize == nbComponents && std::is_same<DataType, ODataType>::value,
				"Both colors must be of the same size and type");
			Color temp{ *this };
			return temp -= other;
		}

		/**
		 * @brief Negate the content of this
		 * @return The negative version of this
		 */
		template<class ODataType, unsigned int oDataSize>
		Color operator-() const
		{
			static_assert(oDataSize == nbComponents && std::is_same<DataType, ODataType>::value,
				"Both colors must be of the same size and type");
			Color temp{};
			return temp -= *this;
		}

		/**
		 * @brief Multiply the content of other by this
		 * @param[in] other The color to multiply by
		 * @return A reference to this
		 */
		template<class ODataType, unsigned int oDataSize>
		Color& operator*=(const Color<ODataType, oDataSize>& other)
		{
			static_assert(oDataSize == nbComponents && std::is_same<DataType, ODataType>::value,
				"Both colors must be of the same size and type");
			for (auto comp = 0; comp < nbComponents; ++comp)
			{
				m_data[comp] *= other[comp];
			}
			return *this;
		}

		/**
		 * @brief Multiply the content of this by other as an immutable object
		 * @param[in] other The color to multiply
		 * @return The multiplication of this by other
		 */
		template<class ODataType, unsigned int oDataSize>
		Color operator*(const Color<ODataType, oDataSize>& other) const
		{
			static_assert(oDataSize == nbComponents && std::is_same<DataType, ODataType>::value,
				"Both colors must be of the same size and type");
			Color temp{ *this };
			return temp *= other;
		}

		/**
		 * @brief Divide the content of this by other
		 * @param[in] other The color to divide
		 * @return A reference to this
		 */
		template<class ODataType, unsigned int oDataSize>
		Color& operator/=(const Color<ODataType, oDataSize>& other)
		{
			static_assert(oDataSize == nbComponents && std::is_same<DataType, ODataType>::value,
				"Both colors must be of the same size and type");
			for (auto comp = 0; comp < nbComponents; ++comp)
			{
				m_data[comp] /= other[comp];
			}
			return *this;
		}

		/**
		 * @brief Divide the content of this by other as an immutable object
		 * @param[in] other The color to divide
		 * @return The division of this by other
		 */
		template<class ODataType, unsigned int oDataSize>
		Color operator/(const Color<ODataType, oDataSize>& other) const
		{
			static_assert(oDataSize == nbComponents && std::is_same<DataType, ODataType>::value,
				"Both colors must be of the same size and type");
			Color temp{ *this };
			return temp /= other;
		}

	private:
		std::array<DataType, nbComponents> m_data{};
	};

	using Colorf = Color<float, 1>;
	using Color2f = Color<float, 2>;
	using Color3f = Color<float, 3>;
	using Color4f = Color<float, 4>;

	using Coloruc = Color<unsigned char, 1>;
	using Color2uc = Color<unsigned char, 2>;
	using Color3uc = Color<unsigned char, 3>;
	using Color4uc = Color<unsigned char, 4>;

	using Colorus = Color<unsigned short, 1>;
	using Color2us = Color<unsigned short, 2>;
	using Color3us = Color<unsigned short, 3>;
	using Color4us = Color<unsigned short, 4>;

} // namespace stbipp
