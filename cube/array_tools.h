#pragma once
#include <array>
#include <utility> //index_sequence, integer_sequence
#include <iostream> // for cout


namespace {

	template<typename T, int N, std::size_t... I>
	constexpr std::array<T, N> create_array(std::index_sequence<I...>) noexcept
	{
		return std::array<T, N>{ I... };
	}


	template<typename T, int N1, int N2, int ... S1, int ... S2>
	constexpr std::array<T, N1 + N2> concat_private(
		const std::array<T, N1>& a, 
		const std::array<T, N2>& b, 
		std::index_sequence<S1...>,
		std::index_sequence<S2...>
	) noexcept 
	{
		return { a[S1]..., b[S2]... };
	};

	template<typename T, int N, int ... S>
	constexpr std::array<T, N> add_private(
		const std::array<T, N>& a, 
		const std::array<T, N>& b, 
		std::index_sequence<S...>
	) noexcept 
	{
		return { a[S] + b[S] ... };
	};

	template<typename T, int N, int ... S>
	constexpr std::array<T, N> add_private(
		const std::array<T, N>& a,
		const T value,
		std::index_sequence<S...>
	) noexcept 
	{
		return { (static_cast<T>(a[S] + value)) ... };
	};

	template<typename T, int N, int ... S>
	constexpr bool equal_private(
		const std::array<T, N>& a, 
		const std::array<T, N>& b, 
		std::index_sequence<S...>
	) noexcept 
	{
		return (... && (a[S] == b[S]));
	};
}

namespace array_tools {

	template <typename T, int N>
	constexpr std::array<T, N> create_index_array() noexcept
	{
		return create_array<T, N>(std::make_index_sequence<N>());
	}

	template <typename T, int N1, int N2>
	constexpr std::array<T, N1 + N2> concat(
		const std::array<T, N1>& a, 
		const std::array<T, N2>& b) noexcept
	{
		constexpr auto s1 = std::make_index_sequence<N1>();
		constexpr auto s2 = std::make_index_sequence<N2>();
		return concat_private(a, b, s1, s2);
	}

	template <typename T, int N1, int N2, int N3>
	constexpr std::array<T, N1 + N2 + N3> concat(
		const std::array<T, N1>& a, 
		const std::array<T, N2>& b, 
		const std::array<T, N3>& c) noexcept
	{
		constexpr auto ab = concat_private(a, b);
		constexpr auto s1 = std::make_index_sequence<N1 + N2>();
		constexpr auto s2 = std::make_index_sequence<N3>();
		return concat_private(ab, c, s1, s2);
	}

	template <typename T, int N1, int N2, int N3, int N4>
	constexpr std::array<T, N1 + N2 + N3 + N4> concat(
		const std::array<T, N1>& a, 
		const std::array<T, N2>& b, 
		const std::array<T, N3>& c, 
		const std::array<T, N4>& d) noexcept
	{
		constexpr auto ab = concat_private(a, b);
		constexpr auto cd = concat_private(c, d);
		constexpr auto s1 = std::make_index_sequence<N1 + N2>();
		constexpr auto s2 = std::make_index_sequence<N3 + N4>();
		return concat_private(ab, cd, s1, s2);
	}

	template <typename T, int N>
	constexpr std::array<T, N> add(
		const std::array<T, N>& a, 
		const std::array<T, N>& b) noexcept
	{
		return add_private(a, b, std::make_index_sequence<N>());
	}

	template <typename T, int N>
	constexpr std::array<T, N> add(
		const std::array<T, N>& a, 
		const T value) noexcept 
	{
		return add_private(a, value, std::make_index_sequence<N>());
	}

	template <typename T, int N>
	constexpr bool equal(
		const std::array<T, N>& a, 
		const std::array<T, N>& b) noexcept 
	{
		return equal_private(a, b, std::make_index_sequence<N>());
	}

	constexpr void test() 
	{
		constexpr auto a = create_index_array<int, 10>();
		constexpr auto b = create_index_array<int, 10>();
		static_assert(equal(a, b));

		constexpr auto c = add(a, 2);
		//constexpr auto d = add(c, 2);

		for (int i = 0; i < c.size(); ++i) {
			std::cout << c[i] << " ";
		}
	}
}