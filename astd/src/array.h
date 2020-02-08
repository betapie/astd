#pragma once

namespace astd
{
	/*
	TODOs:
	Change iterator type when implemented + reverse Iterator
	Implement Swap
	implementation for _Size = 0
	*/

	template<typename T, size_t _Size>
	struct array
	{
		using value_type = T;
		using size_type = size_t;
		using pointer = T*;
		using const_pointer = const T*;
		using reference = T&;
		using const_reference = const T&;
		using iterator = pointer;
		using const_iterator = const_pointer;

		void fill(const T& val) // TODO can this be done at compile time?
		{
			memset(m_data, val, sizeof(T) * _Size);
		}

		[[nodiscard]] inline T& at(size_t idx)
		{
#ifdef HardwareSerial_h
			if (Serial)
				Serial.println("array subscript out of range");
#endif
			return m_data[idx];
		}

		[[nodiscard]] constexpr const T& at(size_t idx) const
		{
#ifdef HardwareSerial_h
			if (Serial)
				Serial.println("array subscript out of range");
#endif
			return m_data[idx];
		}

		[[nodiscard]] inline T& operator[](size_t idx) noexcept
		{
			return m_data[idx];
		}

		[[nodiscard]] constexpr const T& operator[](size_t idx) const noexcept
		{
			return m_data[idx];
		}

		[[nodiscard]] inline T& front() noexcept
		{
			return m_data[0];
		}

		[[nodiscard]] constexpr const T& front() const noexcept
		{
			return m_data[0];
		}

		[[nodiscard]] inline T& back() noexcept
		{
			return m_data[_Size - 1];
		}

		[[nodiscard]] constexpr const T& back() const noexcept
		{
			return m_data[_Size - 1];
		}

		[[nodiscard]] inline T* data() noexcept
		{
			return m_data;
		}

		[[nodiscard]] constexpr const T* data() const noexcept
		{
			return m_data;
		}

		[[nodiscard]] inline iterator begin() noexcept
		{
			return m_data;
		}

		[[nodiscard]] inline const_iterator begin() const noexcept
		{
			return m_data;
		}

		[[nodiscard]] inline iterator end() noexcept
		{
			return m_data + _Size;
		}

		[[nodiscard]] inline const_iterator end() const noexcept
		{
			return m_data + _Size;
		}

		[[nodiscard]] inline const_iterator cbegin() const noexcept
		{
			return m_data;
		}

		[[nodiscard]] inline const_iterator cend() const noexcept
		{
			return m_data + _Size;
		}

		[[nodiscard]] constexpr size_type size() const noexcept
		{
			return _Size;
		}

		[[nodiscard]] constexpr size_type max_size() const noexcept
		{
			return _Size;
		}

		[[nodiscard]] constexpr bool empty() const noexcept
		{
			return false;
		}

		T m_data[_Size];
	};

	template<size_t idx, typename T, size_t _Size>
	[[nodiscard]] constexpr T& get(array<T, _Size>& arr) noexcept
	{
		static_assert(idx < _Size, "array index out of bounds");
		return arr.m_data[idx];
	}

	template<size_t idx, typename T, size_t _Size>
	[[nodiscard]] constexpr const T& get(const array<T, _Size>& arr) noexcept
	{
		static_assert(idx < _Size, "array index out of bounds");
		return arr.m_data[idx];
	}

	template<size_t idx, typename T, size_t _Size>
	[[nodiscard]] constexpr T&& get(array<T, _Size>&& arr) noexcept
	{
		static_assert(idx < _Size, "array index out of bounds");
		return move(arr.m_data[idx]);
	}

	template<size_t idx, typename T, size_t _Size>
	[[nodiscard]] constexpr const T&& get(const array<T, _Size>&& arr) noexcept
	{
		static_assert(idx < _Size, "array index out of bounds");
		return move(arr.m_data[idx]);
	}
}