#pragma once
#include <array>



namespace n_cube {

	using BF = unsigned long long;

	template<int D>
	class Cube {
	public:
		constexpr static int DIM = D;

		constexpr explicit Cube(BF bf) : 
			bf_(bf)
			//data_(to_array(bf)) 
		{};

		constexpr explicit Cube(const std::array<bool, (1 << D)>& d) : 
			bf_(to_bf(d))
			//data_(d) 
		{};

		constexpr bool operator[](int index) const noexcept
		{
			return (this->bf_ >> index) & 1; 
			//return this->data_[index];
		}

		constexpr bool operator==(const Cube<D>& other) const noexcept
		{
			return this->bf_ == other.bf_;
		}
		constexpr BF get_bf() const noexcept
		{
			return this->bf_;
		}

	private:
		using CubeType = bool;

		static std::array<bool, (1 << D)> to_array(BF bf) {
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

		static BF to_bf([[maybe_unused]] const std::array<bool, (1 << D)>& d) {
			if constexpr (D == 1) {
				return
					(d[0] << 1) |
					(d[1] << 0);
			}
			if constexpr (D == 2) {
				return
					(d[0] << 3) |
					(d[1] << 2) |
					(d[2] << 1) |
					(d[3] << 0);
			}
			if constexpr (D == 3) {
				return
					(d[0] << 7) |
					(d[1] << 6) |
					(d[2] << 5) |
					(d[3] << 4) |
					(d[4] << 3) |
					(d[5] << 2) |
					(d[6] << 1) |
					(d[7] << 0);
			}
			if constexpr (D == 4) {
				return
					(d[0] << 15) |
					(d[1] << 14) |
					(d[2] << 13) |
					(d[3] << 12) |
					(d[4] << 11) |
					(d[5] << 10) |
					(d[6] << 9) |
					(d[7] << 8) |
					(d[8] << 7) |
					(d[9] << 6) |
					(d[10] << 5) |
					(d[11] << 4) |
					(d[12] << 3) |
					(d[13] << 2) |
					(d[14] << 1) |
					(d[15] << 0);
			}
			if constexpr (D == 5) {
				return
					(d[0] << 31) |
					(d[1] << 30) |
					(d[2] << 29) |
					(d[3] << 28) |
					(d[4] << 27) |
					(d[5] << 26) |
					(d[6] << 25) |
					(d[7] << 24) |
					(d[8] << 23) |
					(d[9] << 22) |
					(d[10] << 21) |
					(d[11] << 20) |
					(d[12] << 19) |
					(d[13] << 18) |
					(d[14] << 17) |
					(d[15] << 16) |
					(d[16] << 15) |
					(d[17] << 14) |
					(d[18] << 13) |
					(d[19] << 12) |
					(d[20] << 11) |
					(d[21] << 10) |
					(d[22] << 9) |
					(d[23] << 8) |
					(d[24] << 7) |
					(d[25] << 6) |
					(d[26] << 5) |
					(d[27] << 4) |
					(d[28] << 3) |
					(d[29] << 2) |
					(d[30] << 1) |
					(d[31] << 0);
			}
		}

		BF bf_;
		//std::array<bool, (1 << D)> data_;
	};
}