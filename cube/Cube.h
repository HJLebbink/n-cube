#pragma once
#include <array>


namespace cube {

	using BF = unsigned long long;

	template<int D>
	class Cube {
	public:
		constexpr static int DIM = D;

		constexpr explicit Cube(BF bf) noexcept : 
			bf_(bf)
			//data_(to_array(bf)) 
		{};

		constexpr explicit Cube(const std::array<bool, (1 << D)>& d) noexcept : 
			bf_(to_bf(d))
			//data_(d) 
		{};

		[[nodiscard]] constexpr bool operator[](int index) const noexcept
		{
			return (this->bf_ >> index) & 1; 
			//return this->data_[index];
		}

		[[nodiscard]] constexpr bool operator==(const Cube<D>& other) const noexcept
		{
			return this->bf_ == other.bf_;
		}
		[[nodiscard]] constexpr BF get_bf() const noexcept
		{
			return this->bf_;
		}

	private:
		using CubeType = bool;

		static constexpr std::array<bool, (1 << D)> to_array(BF bf) {
			if constexpr (D == 1) {
				return std::array<bool, (1 << D)> {
					static_cast<CubeType>((bf >> 1) & 1),
						static_cast<CubeType>((bf >> 0) & 1)};
			}
			if constexpr (D == 2) {
				return std::array<bool, (1 << D)> {
					static_cast<CubeType>((bf >> 3) & 1),
						static_cast<CubeType>((bf >> 2) & 1),
						static_cast<CubeType>((bf >> 1) & 1),
						static_cast<CubeType>((bf >> 0) & 1)};

			}
			if constexpr (D == 3) {
				return std::array<bool, (1 << D)> {
					static_cast<CubeType>((bf >> 7) & 1),
						static_cast<CubeType>((bf >> 6) & 1),
						static_cast<CubeType>((bf >> 5) & 1),
						static_cast<CubeType>((bf >> 4) & 1),
						static_cast<CubeType>((bf >> 3) & 1),
						static_cast<CubeType>((bf >> 2) & 1),
						static_cast<CubeType>((bf >> 1) & 1),
						static_cast<CubeType>((bf >> 0) & 1)};

			}
			if constexpr (D == 4) {
				return std::array<bool, (1 << D)> {
					static_cast<CubeType>((bf >> 15) & 1),
						static_cast<CubeType>((bf >> 14) & 1),
						static_cast<CubeType>((bf >> 13) & 1),
						static_cast<CubeType>((bf >> 12) & 1),
						static_cast<CubeType>((bf >> 11) & 1),
						static_cast<CubeType>((bf >> 10) & 1),
						static_cast<CubeType>((bf >> 9) & 1),
						static_cast<CubeType>((bf >> 8) & 1),
						static_cast<CubeType>((bf >> 7) & 1),
						static_cast<CubeType>((bf >> 6) & 1),
						static_cast<CubeType>((bf >> 5) & 1),
						static_cast<CubeType>((bf >> 4) & 1),
						static_cast<CubeType>((bf >> 3) & 1),
						static_cast<CubeType>((bf >> 2) & 1),
						static_cast<CubeType>((bf >> 1) & 1),
						static_cast<CubeType>((bf >> 0) & 1)};

			}
			if constexpr (D == 5) {
				return std::array<bool, (1 << D)> {
					static_cast<CubeType>((bf >> 31) & 1),
						static_cast<CubeType>((bf >> 30) & 1),
						static_cast<CubeType>((bf >> 29) & 1),
						static_cast<CubeType>((bf >> 28) & 1),
						static_cast<CubeType>((bf >> 27) & 1),
						static_cast<CubeType>((bf >> 26) & 1),
						static_cast<CubeType>((bf >> 25) & 1),
						static_cast<CubeType>((bf >> 24) & 1),
						static_cast<CubeType>((bf >> 23) & 1),
						static_cast<CubeType>((bf >> 22) & 1),
						static_cast<CubeType>((bf >> 21) & 1),
						static_cast<CubeType>((bf >> 20) & 1),
						static_cast<CubeType>((bf >> 19) & 1),
						static_cast<CubeType>((bf >> 18) & 1),
						static_cast<CubeType>((bf >> 17) & 1),
						static_cast<CubeType>((bf >> 16) & 1),
						static_cast<CubeType>((bf >> 15) & 1),
						static_cast<CubeType>((bf >> 14) & 1),
						static_cast<CubeType>((bf >> 13) & 1),
						static_cast<CubeType>((bf >> 12) & 1),
						static_cast<CubeType>((bf >> 11) & 1),
						static_cast<CubeType>((bf >> 10) & 1),
						static_cast<CubeType>((bf >> 9) & 1),
						static_cast<CubeType>((bf >> 8) & 1),
						static_cast<CubeType>((bf >> 7) & 1),
						static_cast<CubeType>((bf >> 6) & 1),
						static_cast<CubeType>((bf >> 5) & 1),
						static_cast<CubeType>((bf >> 4) & 1),
						static_cast<CubeType>((bf >> 3) & 1),
						static_cast<CubeType>((bf >> 2) & 1),
						static_cast<CubeType>((bf >> 1) & 1),
						static_cast<CubeType>((bf >> 0) & 1)};

			}
		}

		static constexpr BF to_bf([[maybe_unused]] const std::array<bool, (1 << D)>& d) noexcept {
			if constexpr (D == 1) {
				return
					(d[0] << 0) |
					(d[1] << 1);
			}
			if constexpr (D == 2) {
				return
					(d[0] << 0) |
					(d[1] << 1) |
					(d[2] << 2) |
					(d[3] << 3);
			}
			if constexpr (D == 3) {
				return
					(d[0] << 0) |
					(d[1] << 1) |
					(d[2] << 2) |
					(d[3] << 3) |
					(d[4] << 4) |
					(d[5] << 5) |
					(d[6] << 6) |
					(d[7] << 7);
			}
			if constexpr (D == 4) {
				return
					(d[0] << 0) |
					(d[1] << 1) |
					(d[2] << 2) |
					(d[3] << 3) |
					(d[4] << 4) |
					(d[5] << 5) |
					(d[6] << 6) |
					(d[7] << 7) |
					(d[8] << 8) |
					(d[9] << 9) |
					(d[10] << 10) |
					(d[11] << 11) |
					(d[12] << 12) |
					(d[13] << 13) |
					(d[14] << 14) |
					(d[15] << 15);
			}
			if constexpr (D == 5) {
				return
					(d[0] << 0) |
					(d[1] << 1) |
					(d[2] << 2) |
					(d[3] << 3) |
					(d[4] << 4) |
					(d[5] << 5) |
					(d[6] << 6) |
					(d[7] << 7) |
					(d[8] << 8) |
					(d[9] << 9) |
					(d[10] << 10) |
					(d[11] << 11) |
					(d[12] << 12) |
					(d[13] << 13) |
					(d[14] << 14) |
					(d[15] << 15) |
					(d[16] << 16) |
					(d[17] << 17) |
					(d[18] << 18) |
					(d[19] << 19) |
					(d[20] << 20) |
					(d[21] << 21) |
					(d[22] << 22) |
					(d[23] << 23) |
					(d[24] << 24) |
					(d[25] << 25) |
					(d[26] << 26) |
					(d[27] << 27) |
					(d[28] << 28) |
					(d[29] << 29) |
					(d[30] << 30) |
					(d[31] << 31);
			}
		}

		BF bf_;
		//std::array<bool, (1 << D)> data_;
	};
}