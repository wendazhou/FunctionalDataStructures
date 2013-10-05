#ifndef WENDA_FDS_DETAIL_REDBLACK_TREE_REDUCE_H_INCLUDED
#define WENDA_FDS_DETAIL_REDBLACK_TREE_REDUCE_H_INCLUDED

/**
* @file redblack_tree_reduce.h
* This file implements the basic reduction function
* for red-black nodes.
*/

#include "../FDS_common.h"
#include "redblack_tree_data.h"

#include <type_traits>

WENDA_FDS_NAMESPACE_BEGIN

namespace detail
{
	/**
	* Recursively reduces the given @p node in order using the given reduction @p function and @p seed.
	* @param node The node to reduce.
	* @param function The aggregation function.
	* @param seed The initial value to pass to the aggregation function.
	*/
	template<typename T, typename Function, typename Seed>
	typename std::decay<Seed>::type reduce(redblack_node<T> const& node, Function&& function, Seed seed)
	{
		if (node.get_left())
		{
			seed = reduce(*node.get_left(), function, std::move(seed));
		}
		
		seed = function(std::move(seed), node.get_data());

		if (node.get_right())
		{
			seed = reduce(*node.get_right(), function, std::move(seed));
		}

		return seed;
	}
}

WENDA_FDS_NAMESPACE_END

#endif // WENDA_FDS_DETAIL_REDBLACK_TREE_REDUCE_H_INCLUDED