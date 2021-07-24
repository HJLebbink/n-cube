module;

export module constants;

namespace cube {
	export inline constexpr unsigned long long cardinality_npn_class(int n) {
		switch (n) {
		case 1: return 2;
		case 2: return 4;
		case 3: return 14;
		case 4: return 222;
		case 5: return 616126;
		case 6: return 200253952537184;
		default: return 0;
		}
	}
}
