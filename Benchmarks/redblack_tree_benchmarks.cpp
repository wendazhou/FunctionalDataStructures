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
		: data(element_count), trees(element_count)
	{
		std::mt19937 mt(145);
		std::uniform_int_distribution<int> dis;

		std::generate_n(data.begin(), element_count, [&](){ return dis(mt); });
	}

	std::vector<int> data;
	std::vector<fds::redblack_tree<int>> trees;
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

BENCHMARK_F(RedBlackTreeInsert, FDS_RedBlackTree_Insert_Keep_All, RedBlackTreeInsertFixture, 0, 10)
{
	fds::redblack_tree<int> set;

	for (size_t i = 0; i < element_count; i++)
	{
		std::tie(set, std::ignore, std::ignore) = set.insert(data[i]);
		trees[i] = set;
	}
}

class RedBlackTreeFindDeleteFixture
	: public celero::TestFixture
{
public:
	RedBlackTreeFindDeleteFixture()
		: removed(element_count)
	{
		std::mt19937  mt(123);
		std::uniform_int_distribution<int> dis;

		for (std::size_t i = 0; i < element_count; i++)
		{
			int value = dis(mt);
			data.push_back(value);
			std::tie(fds_tree, std::ignore, std::ignore) = fds_tree.insert(value);
			std_set.insert(value);
		}
	}

	std::vector<int> data;
	fds::redblack_tree<int> fds_tree;
	std::set<int> std_set;

	std::vector<fds::redblack_tree<int>> removed;

	static const std::size_t element_count = 1000;
};

BASELINE_F(RedBlackTreeFind, STD_Set_Find, RedBlackTreeFindDeleteFixture, 0, 1000)
{
	for (std::size_t i = 0; i < element_count; i++)
	{
		data[i] = *std_set.find(data[i]);
	}
}

BENCHMARK_F(RedBlackTreeFind, FDS_RedBlackTree_Find, RedBlackTreeFindDeleteFixture, 0, 1000)
{
	for (std::size_t i = 0; i < element_count; i++)
	{
		data[i] = *fds_tree.find(data[i]);
	}
}

BASELINE_F(RedBlackTreeDelete, STD_Set_Delete, RedBlackTreeFindDeleteFixture, 0, 10)
{
	std::set<int> set = std_set;

	for (auto i : data)
	{
		set.erase(i);
	}
}

BENCHMARK_F(RedBlackTreeDelete, FDS_RedBlackTree_Delete_Keep_One, RedBlackTreeFindDeleteFixture, 0, 10)
{
	fds::redblack_tree<int> tree = fds_tree;

	for (auto i : data)
	{
		std::tie(tree, std::ignore) = tree.erase(i);
	}
}

BENCHMARK_F(RedBlackTreeDelete, FDS_RedBlackTree_Delete_Keep_All, RedBlackTreeFindDeleteFixture, 0, 10)
{
	fds::redblack_tree<int> tree = fds_tree;

	for (std::size_t i = 0; i < element_count; i++)
	{
		std::tie(tree, std::ignore) = tree.erase(data[i]);
		removed[i] = tree;
	}
}