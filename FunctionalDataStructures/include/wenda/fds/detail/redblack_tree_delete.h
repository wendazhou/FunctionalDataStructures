#ifndef WENDA_FDS_REDBLACK_TREE_DELETE_H_INCLUDED
#define WENDA_FDS_REDBLACK_TREE_DELETE_H_INCLUDED

/**
* @file redblack_tree_delete.h
* This file implements the deletion operation for red black trees.
*/

#include "../FDS_common.h"
#include "redblack_tree_data.h"
#include "redblack_tree_balance.h"

WENDA_FDS_NAMESPACE_BEGIN

namespace detail
{
	/**
	* Extended balance operation as described in http://matt.might.net/articles/red-black-delete/ to balance
	* trees after a bubble operation. This handles double blacks and negative blacks, and may recursively call itself
	* once in the case of a negative black.
	* @sa balance()
	*/
	template<typename T, typename U>
	intrusive_rb_ptr<T> bubble_balance(NodeColour node_colour, U&& value, const_intrusive_rb_ptr<T> left, const_intrusive_rb_ptr<T> right)
	{
		if (node_colour == NodeColour::DoubleBlack)
		{
			const_rb_pointer<T> dummy;

			if (colour(left) == NodeColour::NegativeBlack)
			{
				assert(left->get_left());
				assert(left->get_right());
				assert(colour(left->get_left()) == NodeColour::Black);
				assert(colour(left->get_right()) == NodeColour::Black);

				auto newLeft = balance<T>(NodeColour::Black, left->get_data(), make_red(left->get_left().get()), left->get_right()->get_left(), dummy);
				auto newRight = make_redblack_node<T>(std::forward<U>(value), NodeColour::Black, left->get_right(), std::move(right));
				return make_redblack_node<T>(left->get_right()->get_data(), NodeColour::Black, std::move(newLeft), std::move(newRight));
			}
			else if (colour(right) == NodeColour::NegativeBlack)
			{
				assert(right->get_left());
				assert(right->get_right());
				assert(colour(right->get_left()) == NodeColour::Black);
				assert(colour(right->get_right()) == NodeColour::Black);

				auto newRight = balance<T>(NodeColour::Black, right->get_data(), make_red(right->get_right().get()), right->get_left()->get_right(), dummy);
				auto newLeft = make_redblack_node<T>(std::forward<U>(value), NodeColour::Black, right->get_left(), std::move(left));
				return make_redblack_node<T>(right->get_left()->get_data(), NodeColour::Black, std::move(newLeft), std::move(newRight));
			}
		}

		const_rb_pointer<T> dummy = nullptr;
		return balance(node_colour, std::forward<U>(value), std::move(left), std::move(right), dummy);
	}

	template<typename T, typename U>
	intrusive_rb_ptr<T> bubble(NodeColour node_colour, U&& value, const_intrusive_rb_ptr<T> left, const_intrusive_rb_ptr<T> right)
	{
		if (colour(left) == NodeColour::DoubleBlack || colour(right) == NodeColour::DoubleBlack)
		{
			const_rb_pointer<T> inserted_dummy = nullptr;
			set_colour(left, colour(left) - NodeColour::Black);
			set_colour(right, colour(right) - NodeColour::Black);

			return balance(node_colour + NodeColour::Black, std::forward<U>(value), std::move(left), std::move(right), inserted_dummy);
		}
		else
		{
			return make_redblack_node<T>(std::forward<U>(value), node_colour, std::move(left), std::move(right));
		}
	}

	template<typename T>
	const_intrusive_rb_ptr<T> remove_node(const_rb_pointer<T> node)
	{
		if (node->get_left() && node->get_right())
		{
			// two children case
			// remove the maximum descendant of the left child,
			// and write its value into the current node.
			auto max = node->get_left()->maximum();

			auto removed = remove_node(max);

			auto newLeft = make_redblack_node<T>(node->get_left()->get_data(), colour(node->get_left()), node->get_left()->get_left(), removed);

			return make_redblack_node<T>(max->get_data(), colour(node), newLeft, node->get_right());
		}
		else if (node->get_left() || node->get_right())
		{
			// one child case. The child must be red, the parent black (otherwise violates RB condition).
			auto child = node->get_left() ? node->get_left() : node->get_right();

			set_colour(child, NodeColour::Black);

			return child;
		}
		else
		{
			// no children
			// return an empty node with the correct colour.
			return make_null_redblack_node<T>(colour(node) + NodeColour::Black);
		}
	}

    template<typename T, typename U, typename Compare>
	std::tuple<const_intrusive_rb_ptr<T>, bool> 
	find_delete_node(const_rb_pointer<T> tree, U&& value, Compare const& compare)
	{
		if (!tree)
		{
			return std::make_tuple(make_null_redblack_node<T>(), false);
		}

		const_intrusive_rb_ptr<T> newTree;
		bool deleted;

		if (compare(value, tree->get_data()))
		{
			std::tie(newTree, deleted) = find_delete_node(tree->get_left().get(), std::forward<U>(value), compare);
		}
		else if (compare(tree->get_data(), value))
		{
			std::tie(newTree, deleted) = find_delete_node(tree->get_right().get(), std::forward<U>(value), compare);
		}
		else
		{
			newTree = remove_node(std::move(tree));
			deleted = true;
		}

		typedef std::tuple<const_intrusive_rb_ptr<T>, bool> return_t;
		return return_t(newTree, deleted);
	}
}

WENDA_FDS_NAMESPACE_END

#endif // WENDA_FDS_REDBLACK_TREE_DELETE_H_INCLUDED