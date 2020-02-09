#pragma once
#include "type_traits.h"
#include "memory.h"

namespace astd
{
	template <typename return_type, typename... Args>
	class invocable_base
	{
	public:
		virtual ~invocable_base() = default;
		virtual return_type Invoke(Args&&...) = 0;

	private:
	};

	template<typename T, typename return_type, typename... Args>
	class invocable : public invocable_base<return_type, Args...>
	{
	public:
		invocable(T&& t) noexcept
			: m_t(forward<T>(t))
		{}

		~invocable() override = default;

		return_type Invoke(Args&&... args) override
		{
			return m_t(forward<Args...>(args...));
		}

	private:
		T m_t;
	};

	template <typename>
	class simple_function;

	template<typename return_type, typename... Args>
	class simple_function<return_type(Args...)>
	{
	public:
		simple_function() noexcept = default;

		simple_function(const simple_function& other) = delete;
		simple_function& operator=(const simple_function& other) = delete;

		simple_function(simple_function&& other) noexcept
			:m_pInvocable(move(other.m_pInvocable))
		{}

		simple_function& operator=(simple_function&& other) noexcept
		{
			if (this == &other)
				return *this;

			m_pInvocable = move(other.m_pInvocable);
			return *this;
		}

		template<typename T>
		simple_function(T&& t)
			:m_pInvocable(make_unique<invocable<T, return_type, Args...>>(forward<T>(t)))
		{
		}

		template<typename T>
		simple_function& operator=(T&& t)
		{
			m_pInvocable = make_unique<invocable<T, return_type, Args...>>(forward<T>(t));
			return *this;
		}

		return_type operator()(Args&&... args) const
		{
			if (m_pInvocable)
				return m_pInvocable->Invoke(forward<Args...>(args...));
		}

	private:
		unique_ptr<invocable_base<return_type, Args...>> m_pInvocable = nullptr;
	};
}