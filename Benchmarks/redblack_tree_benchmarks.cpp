#include <cstddef>

#include <random>

#include <set>
#include <vector>

#include <algorithm>

#include <celero/Celero.h>
#include <wenda/fds/redblack_tree.h>

using namespace wenda;

class RedBlackTreeInsertFixture
	: public celero::TestFixture
{
public:
	RedBlackTreeInsertFixture()
		: data(element_count)
	{
		std::mt19937 mt(145);
		std::uniform_int_distribution<int> dis;

		std::generate_n(data.begin(), element_count, [&](){ return dis(mt); });
	}

	std::vector<int> data;
	static const std::size_t element_count = 10000;
};

BASELINE_F(RedBlackTreeInsert, STD_Set_Insert, RedBlackTreeInsertFixture, 0, 10)
{
	std::set<int> set;

	for (size_t i = 0; i < element_count; i++)
	{
		set.insert(data[i]);
	}

	celero::DoNotOptimizeAway(set);
}

BENCHMARK_F(RedBlackTreeInsert, FDS_RedBlackTree_Insert_Keep_One, RedBlackTreeInsertFixture, 0, 10)
{
	fds::redblack_tree<int> set;

	for (size_t i = 0; i < element_count; i++)
	{
		std::tie(set, std::ignore, std::ignore) = set.insert(data[i]);
	}

	celero::DoNotOptimizeAway(set);
}