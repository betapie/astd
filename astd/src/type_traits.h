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

    template<typename T, typename... Ts>
    constexpr auto conjunction_v = conjunction<T, Ts...>::value;

    template<typename...>
    struct disjunction : false_type
    {};

    template<typename T>
    struct disjunction<T> : T
    {};

    template<typename T, typename... Ts>
    struct disjunction<T, Ts...> : conditional_t<bool(T::value), T, disjunction<Ts...>>
    {};

    template<typename T, typename... Ts>
    constexpr auto disjunction_v = disjunction<T, Ts...>::value;

    template<typename T>
    struct negation : bool_constant<!bool(T::value)>
    {};

    template<typename T>
    constexpr auto negation_v = negation<T>::value;

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
    using is_default_constructible = is_constructible<T>;

    template<typename T>
    constexpr auto is_default_constructible_v = is_default_constructible<T>::value;

    template<typename T>
    using is_copy_constructible = is_constructible<T, add_lvalue_reference_t<const T>>;

    template<typename T>
    constexpr auto is_copy_constructible_v = is_copy_constructible<T>::value;

    template<typename T>
    using is_move_constructible = is_constructible<T, T>;

    template<typename T>
    constexpr auto is_move_constructible_v = is_move_constructible<T>::value;

    template<typename, typename T, typename U>
    struct is_assignable_impl : false_type
    {};

    template<typename T, typename U>
    struct is_assignable_impl<void_t<decltype(declval<T>() = declval<U>())>, T, U> : true_type
    {};

    template<typename T, typename U>
    using is_assignable = is_assignable_impl<void_t<>, T, U>;

    template<typename T, typename U>
    constexpr auto is_assignable_v = is_assignable<T, U>::value;

    template<typename T>
    using is_copy_assignable = is_assignable<add_lvalue_reference_t<T>, add_lvalue_reference_t<const T>>;

    template<typename T>
    constexpr auto is_copy_assignable_v = is_copy_assignable<T>::value;

    template<typename T>
    using is_move_assignable = is_assignable<add_lvalue_reference_t<T>, T>;

    template<typename T>
    constexpr auto is_move_assignable_v = is_move_assignable<T>::value;

    template<typename T, typename U>
    struct is_same : false_type
    {};

    template<typename T>
    struct is_same<T, T> : true_type
    {};

    template<typename T, typename U>
    constexpr auto is_same_v = is_same<T, U>::value;

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
    constexpr auto is_array_v = is_array<T>::value;

    template<typename T>
    struct is_const : false_type
    {};

    template<typename T>
    struct is_const<const T> : true_type
    {};

    template<typename T>
    constexpr auto is_const_v = is_const<T>::value;

    template<typename T>
    struct is_reference : false_type
    {};

    template<typename T>
    struct is_reference<T&> : true_type
    {};

    template<typename T>
    struct is_reference<T&&> : true_type
    {};

    template<typename T>
    constexpr auto is_reference_v = is_reference<T>::value;

    template<typename T>
    struct is_function : bool_constant<!is_const_v<const T> && !is_reference_v<T>>
    {};

    template<typename T>
    constexpr auto is_function_v = is_function<T>::value;

    template<typename T>
    struct remove_const
    {
        using type = T;
    };

    template<typename T>
    struct remove_const<const T>
    {
        using type = T;
    };

    template<typename T>
    using remove_const_t = typename remove_const<T>::type;

    template<typename T>
    struct remove_volatile
    {
        using type = T;
    };

    template<typename T>
    struct remove_volatile<volatile T>
    {
        using type = T;
    };

    template<typename T>
    using remove_volatile_t = typename remove_volatile<T>::type;

    template<typename T>
    struct remove_cv
    {
        using type = T;
    };

    template<typename T>
    struct remove_cv<const T>
    {
        using type = T;
    };

    template<typename T>
    struct remove_cv<volatile T>
    {
        using type = T;
    };

    template<typename T>
    struct remove_cv<const volatile T>
    {
        using type = T;
    };

    template<typename T>
    using remove_cv_t = typename remove_cv<T>::type;

    template<typename T>
    struct is_void : is_same<void, remove_cv_t<T>>
    {};

    template<typename T>
    constexpr auto is_void_v = is_void<T>::value;

    template<typename T>
    struct is_pointer_impl : false_type
    {};

    template<typename T>
    struct is_pointer_impl<T*> : true_type
    {};

    template<typename T>
    struct is_pointer : is_pointer_impl<remove_cv_t<T>>
    {};

    template<typename T>
    constexpr auto is_pointer_t = is_pointer<T>::value;

    template<typename, typename From, typename To>
    struct is_nonvoid_convertible_impl : false_type
    {};

    template<typename From, typename To>
    struct is_nonvoid_convertible_impl<void_t<decltype(declval<void(&)(To)>()(declval<From>()))>, From, To> : true_type
    {};

    template<typename From, typename To>
    struct is_convertible : disjunction<is_nonvoid_convertible_impl<void_t<>, From, To>, conjunction<is_void<From>, is_void<To>>>
    {};

    template<typename From, typename To>
    constexpr auto is_convertible_v = is_convertible<From, To>::value;

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
    struct remove_extent
    {
        using type = T;
    };

    template<typename T>
    struct remove_extent<T[]>
    {
        using type = T;
    };

    template<typename T, size_t _size>
    struct remove_extent<T[_size]>
    {
        using type = T;
    };

    template<typename T>
    using remove_extent_t = typename remove_extent<T>::type;

    template<typename T, typename = void>
    struct add_pointer_impl
    {
        using type = T;
    };

    template<typename T>
    struct add_pointer_impl<T, void_t<remove_reference_t<T>*>>
    {
        using type = remove_reference_t<T>*;
    };

    template<typename T>
    struct add_pointer
    {
        using type = typename add_pointer_impl<T>::type;
    };

    template<typename T>
    using add_pointer_t = typename add_pointer<T>::type;

    template<typename T>
    struct decay
    {
        using type = conditional_t<is_function_v<remove_reference_t<T>>, add_pointer_t<remove_reference_t<T>>,
            conditional_t<is_array_v<remove_reference_t<T>>, add_pointer_t<remove_extent_t<remove_reference_t<T>>>,
            remove_cv_t<remove_reference_t<T>>>>;
    };

    template<typename T>
    using decay_t = typename decay<T>::type;

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