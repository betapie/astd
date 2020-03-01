#pragma once
#ifndef ASTD_TYPETRAITS
#define ASTD_TYPETRAITS

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

    template<bool _test, typename T, typename U>
    using conditional_t = typename conditional<_test, T, U>::type;

    template<typename...>
    using void_t = void;

    template<typename...>
    struct conjunction : true_type
    {};

    template<typename T>
    struct conjunction<T> : T
    {};

    template<typename T, typename... Ts>
    struct conjunction<T, Ts...> : conditional_t<bool(T::value), conjunction<Ts...>, T>
    {};

    template<typename T, typename = void>
    struct add_lvalue_reference_impl
    {
        using type = T;
    };

    template<typename T>
    struct add_lvalue_reference_impl<T, void_t<T&>>
    {
        using type = T&;
    };

    template<typename T>
    struct add_lvalue_reference
    {
        using type = typename add_lvalue_reference_impl<T>::type;
    };

    template<typename T>
    using add_lvalue_reference_t = typename add_lvalue_reference<T>::type;

    template<typename T, typename = void>
    struct add_rvalue_reference_impl
    {
        using type = T;
    };

    template<typename T>
    struct add_rvalue_reference_impl<T, void_t<T&&>>
    {
        using type = T&&;
    };

    template<typename T>
    struct add_rvalue_reference
    {
        using type = typename add_rvalue_reference_impl<T>::type;
    };

    template<typename T>
    using add_rvalue_reference_t = typename add_rvalue_reference<T>::type;

    template<typename T>
    typename add_rvalue_reference<T>::type declval() noexcept;

    template<typename, typename T, typename... Args>
    struct is_constructible_impl : false_type
    {};

    template<typename T, typename... Args>
    struct is_constructible_impl<void_t<decltype(T(declval<Args>()...))>, T, Args...> : true_type
    {};

    template<typename T, typename... Args>
    using is_constructible = is_constructible_impl<void_t<>, T, Args...>;

    template<typename T, typename... Args>
    constexpr auto is_constructible_v = is_constructible<T, Args...>::value;

    template<typename T>
    using is_default_constructible = is_constructible_impl<void_t<>, T>;

    template<typename T>
    constexpr auto is_default_constructible_v = is_default_constructible<T>::value;

    template<typename T>
    using is_copy_constructible = is_constructible_impl<void_t<>, T, add_lvalue_reference_t<const T>>;

    template<typename T>
    constexpr auto is_copy_constructible_v = is_copy_constructible<T>::value;

    template<typename, typename From, typename To>
    struct is_convertible_impl : false_type
    {};

    // I am not sure this is standard conform but this will have to do for the moment
    template<typename From, typename To>
    struct is_convertible_impl<void_t<decltype((To)declval<From>())>, From, To> : true_type
    {};

    template<typename From, typename To>
    using is_convertible = is_convertible_impl<void_t<>, From, To>;

    template<typename From, typename To>
    constexpr auto is_convertible_v = is_convertible<From, To>::value;

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

    template<typename T>
    struct remove_reference
    {
        using type = T;
    };

    template<typename T>
    struct remove_reference<T&>
    {
        using type = T;
    };

    template<typename T>
    struct remove_reference<T&&>
    {
        using type = T;
    };

    template<typename T>
    using remove_reference_t = typename remove_reference<T>::type;

    template<typename T>
    constexpr T&& forward(typename remove_reference<T>::type& t)
    {
        return static_cast<T&&>(t);
    }

    template<typename T>
    constexpr T&& forward(typename remove_reference<T>::type&& t)
    {
        return static_cast<T&&>(t);
    }

    template<typename T>
    constexpr remove_reference_t<T>&& move(T&& object) noexcept
    {
        return static_cast<remove_reference_t<T>&&>(object);
    }
}

#endif // ASTD_TYPETRAITS