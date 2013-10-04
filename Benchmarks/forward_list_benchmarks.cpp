#include <cstddef>
#include <forward_list>

#include <celero/Celero.h>
#include <wenda/fds/forward_list.h>

using namespace wenda;

class ForwardListPushFront
	: public celero::TestFixture
{
public:
	ForwardListPushFront()
		: lists(element_count)
	{}

	std::vector<fds::forward_list<int>> lists;
	static const std::size_t element_count = 1000;
};

BASELINE_F(ForwardList_PushFront, STD_forward_list, ForwardListPushFront, 0, 100)
{
	std::forward_list<int> list;

	for (size_t i = 0, end = element_count; i < end; i++)
	{
		list.push_front(static_cast<int>(i));
	}

	celero::DoNotOptimizeAway(list);
}

BENCHMARK_F(ForwardList_PushFront, FDS_forward_list_keep_one, ForwardListPushFront, 0, 100)
{
	fds::forward_list<int> list;

	for (size_t i = 0, end = element_count; i < end; i++)
	{
		list = list.push_front(static_cast<int>(i));
	}

	celero::DoNotOptimizeAway(list);
}

BENCHMARK_F(ForwardList_PushFront, FDS_forward_list_keep_all, ForwardListPushFront, 0, 100)
{
	fds::forward_list<int> list;

	for (size_t i = 0, end = element_count; i < end; i++)
	{
		this->lists[i] = list.push_front(static_cast<int>(i));
	}

	celero::DoNotOptimizeAway(list);
}