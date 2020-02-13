#pragma once

namespace astd
{
    template<typename T, T val>
    struct integral_constant
    {
        static constexpr T value = val;

        using value_type = T;
        using type = integral_constant;

        constexpr operator value_type() const noexcept
        {
            return value;
        }

        [[nodiscard]] constexpr value_type operator()() const noexcept
        {
            return value;
        }
    };

    template<bool val>
    using bool_constant = integral_constant<bool, val>;

    using true_type = bool_constant<true>;
    using false_type = bool_constant<false>;

    template<bool _test, typename T = void>
    struct enable_if
    {};

    template<typename T>
    struct enable_if<true, T>
    {
        using type = T;
    };

    template<bool _test, typename T = void>
    using enable_if_t = typename enable_if<_test, T>::type;

    template<bool _test, typename T, typename U>
    struct conditional
    {
        using type = T;
    };

    template<typename T, typename U>
    struct conditional<false, T, U>
    {
        using type = U;
    };

    // compile time type checks
    template<typename T, typename U>
    struct is_same : false_type
    {};

    template<typename T>
    struct is_same<T, T> : true_type
    {};

    template<typename T>
    struct is_array : false_type
    {};

    template<typename T>
    struct is_array<T[]> : true_type
    {};

    template<typename T, size_t _size>
    struct is_array<T[_size]> : true_type
    {};

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
    using remove_reference_t = typename remove_reference<T>::type;

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
    constexpr remove_reference_t<T>&& move(T&& object) noexcept
    {
        return static_cast<remove_reference_t<T>&&>(object);
    }
}