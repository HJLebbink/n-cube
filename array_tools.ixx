module;
#include <array>
#include <utility> //index_sequence, integer_sequence
#include <iostream> // for cout
#include <type_traits>

export module array_tools;
//import std.core;


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

	template<typename T, int N, int P = 0>
	constexpr int lesseq_private(
		const std::array<T, N>& a,
		const std::array<T, N>& b
	) noexcept
	{
		if constexpr (P == N - 1) {
			return (a[P] < b[P]) ? 1 : ((a[P] == b[P]) ? 0 : -1);
		}
		else {
			return (a[P] < b[P]) ? 1 : lesseq_private<T, N, P + 1>(a, b);
		}
	};


	template<typename T1, typename T2, int N, int P = 0>
	constexpr void cast_to(
		const std::array<T1, N>& a,
		std::array<T2, N>& b
	) noexcept
	{
		if constexpr (P == N - 1) {
			b[P] = static_cast<T2>(a[P]);
		}
		else {
			b[P] = static_cast<T2>(a[P]);
			cast_to<T1, T2, N, P + 1>(a, b);
		}
	};

	template<typename T, int N>
	bool lesseq_private_slow(
		const std::array<T, N>& a,
		const std::array<T, N>& b
	) noexcept
	{
		std::array<char, N> a2;
		cast_to<T, char>(a, a2);

		std::array<char, N> b2;
		cast_to<T, char>(b, b2);

		const std::string_view sa(a2.data(), sizeof a2);
		const std::string_view sb(b2.data(), sizeof b2);
		return sa < sb;
	};
}

export namespace array_tools {

	template <typename T, int N>
	[[nodiscard]] consteval std::array<T, N> create_index_array() noexcept
	{
		return create_array<T, N>(std::make_index_sequence<N>());
	}

	template <typename T, int N1, int N2>
	[[nodiscard]] constexpr std::array<T, N1 + N2> concat(
		const std::array<T, N1>& a,
		const std::array<T, N2>& b) noexcept
	{
		constexpr auto s1 = std::make_index_sequence<N1>();
		constexpr auto s2 = std::make_index_sequence<N2>();
		return concat_private(a, b, s1, s2);
	}

	template <typename T, int N1, int N2, int N3>
	[[nodiscard]] constexpr std::array<T, N1 + N2 + N3> concat(
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
	[[nodiscard]] constexpr std::array<T, N1 + N2 + N3 + N4> concat(
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
	[[nodiscard]] constexpr std::array<T, N> add(
		const std::array<T, N>& a,
		const std::array<T, N>& b) noexcept
	{
		return add_private(a, b, std::make_index_sequence<N>());
	}

	template <typename T, int N>
	[[nodiscard]] constexpr std::array<T, N> add(
		const std::array<T, N>& a,
		const T value) noexcept
	{
		return add_private(a, value, std::make_index_sequence<N>());
	}

	template <typename T, int N>
	[[nodiscard]] constexpr bool equal(
		const std::array<T, N>& a,
		const std::array<T, N>& b) noexcept
	{
		return equal_private(a, b, std::make_index_sequence<N>());

	}

	template <int N>
	[[nodiscard]] bool lesseq(
		const std::array<int, N>& a,
		const std::array<int, N>& b) noexcept
	{
		return lesseq_private_slow(a, b);
	}

	 void test()
	{
		constexpr auto a = create_index_array<int, 10>();
		constexpr auto b = create_index_array<int, 10>();
		static_assert(equal(a, b));


		constexpr auto x1 = std::array<int, 4>{1, 1, 2, 3};
		constexpr auto x2 = std::array<int, 4>{0, 2, 3, 0};
		[[maybe_unused]] const auto r = lesseq(x1, x2);

		constexpr auto c = add(a, 2);
		//constexpr auto d = add(c, 2);

		for (int i = 0; i < c.size(); ++i) {
			std::cout << c[i] << " ";
		}
	}
}