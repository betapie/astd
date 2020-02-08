#pragma once

namespace astd
{
	template <typename T>
	struct remove_reference
	{
		using type = T;
	};

	template <typename T>
	struct remove_reference<T&>
	{
		using type = T;
	};

	template <typename T>
	struct remove_reference<T&&>
	{
		using type = T;
	};

	template <typename T>
	constexpr T&& forward(typename remove_reference<T>::type& t)
	{
		return static_cast<T&&>(t);
	}

	template <typename T>
	constexpr T&& forward(typename remove_reference<T>::type&& t)
	{
		return static_cast<T&&>(t);
	}

	template <typename T>
	using remove_reference_t = typename remove_reference<T>::type;

	template <typename T>
	constexpr remove_reference_t<T>&& move(T&& object) noexcept
	{
		return static_cast<remove_reference_t<T>&&>(object);
	}
}