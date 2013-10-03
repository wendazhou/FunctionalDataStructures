#ifndef WENDA_FDS_DETAIL_REDBLACK_TREE_BALANCE_H_INCLUDED
#define WENDA_FDS_DETAIL_REDBLACK_TREE_BALANCE_H_INCLUDED

/**
* @file redblack_tree_balance.h
* This file implements the balancing algorithm for red black trees.
*/

#include "../FDS_common.h"
#include "redblack_tree_data.h"

WENDA_FDS_NAMESPACE_BEGIN

namespace detail
{
	/**
	* Helper function for the balance() function.
	* This rebalances the tree by creating the left and right nodes of a balanced red node, from the children
	* of the respective left and right nodes.
	* @param leftLeft The left child of the left node.
	* @param leftRight The right child of the left node.
	* @param rightLeft The left child of the right node.
	* @param rightRight The right child of the right node.
	* @param valueLeft The value of the left node.
	* @param valueRight The value of the right node.
	* @returns A tuple containing the left child as its first element and the right child as its second.
	* @tparam T The type of the element held by the nodes.
	*/
	template<typename T, typename LL, typename LR, typename RL, typename RR, typename ValueL, typename ValueR>
	std::tuple<intrusive_rb_ptr<T>, intrusive_rb_ptr<T>>
		balance_create_leftright(LL&& leftLeft, LR&& leftRight, RL&& rightLeft, RR&& rightRight,
		ValueL&& valueLeft, ValueR&& valueRight)
	{
		auto newLeft = make_redblack_node<T>(
			std::forward<ValueL>(valueLeft), NodeColour::Black,
			std::forward<LL>(leftLeft), std::forward<LR>(leftRight));

		auto newRight = make_redblack_node<T>(
			std::forward<ValueR>(valueRight), NodeColour::Black,
			std::forward<RL>(rightLeft), std::forward<RR>(rightRight));

		typedef std::tuple<intrusive_rb_ptr<T>, intrusive_rb_ptr<T>> return_t;

		return return_t(std::move(newLeft), std::move(newRight));
		}

	/**
	* Helper function for balance(), creates a red node from the given left and right black nodes.
	* @param previousColour The original colour of the ancestor.
	* @param left The left child of the node to be created.
	* @param right The right child of the node to be created.
	* @param value The value of the node to be created.
	* @tparam T The type of the element held by the nodes.
	*/
	template<typename T, typename Left, typename Right, typename Value>
	intrusive_rb_ptr<T> balance_create_middle(NodeColour previousColour, Left&& left, Right&& right, Value&& value)
	{
		return make_redblack_node<T>(std::forward<Value>(value), previousColour - NodeColour::Black,
			std::forward<Left>(left), std::forward<Right>(right));
	}

	template<typename T>
	const_rb_pointer<T> adjust_inserted(const_rb_pointer<T> inserted, const_rb_pointer<T> old_node, const_rb_pointer<T> new_node)
	{
		if (inserted == old_node)
		{
			return new_node;
		}

		return inserted;
	}


	/**
	* Tree balancing operation as described by Okasaki in "Red-Black trees in a functional setting",
	* generalized to handle the case of double blacks (but not negative blacks).
	* This operation rebalances from the point of view of the grandparent of a node with a red parent
	* that has been inserted.
	* @param colour The colour of the grandparent node.
	* @param value The value of the grandparent node.
	* @param left The left child of the grandparent node.
	* @param right The right child of the grandparent node.
	* @param[in,out] inserted A pointer to the node that has just been inserted.
	* It will be fixed up if necessary (if the rebalancing has affected the pointed-to node).
	* @returns A pointer to an equivalent rebalanced tree.
	*/
	template<typename T, typename U>
	intrusive_rb_ptr<T> balance(NodeColour node_colour, U&& value,
		const_intrusive_rb_ptr<T> left, const_intrusive_rb_ptr<T> right,
		const_rb_pointer<T>& inserted)
	{
		if (node_colour != NodeColour::Black && node_colour != NodeColour::DoubleBlack)
		{
			return make_redblack_node<T>(std::forward<U>(value), node_colour, std::move(left), std::move(right));
		}

		if (left && colour(left) == NodeColour::Red)
		{
			if (left->get_left() && colour(left->get_left()) == NodeColour::Red)
			{
				intrusive_rb_ptr<T> newLeft, newRight;

				std::tie(newLeft, newRight) = balance_create_leftright<T>(
					left->get_left()->get_left(), left->get_left()->get_right(), left->get_right(), std::move(right),
					left->get_left()->get_data(), std::forward<U>(value));

				inserted = adjust_inserted<T>(inserted, left->get_left().get(), newLeft.get());

				return balance_create_middle<T>(node_colour, std::move(newLeft), std::move(newRight), left->get_data());
			}
			else if (left->get_right() && colour(left->get_right()) == NodeColour::Red)
			{
				intrusive_rb_ptr<T> newLeft, newRight;

				std::tie(newLeft, newRight) = balance_create_leftright<T>(
					left->get_left(), left->get_right()->get_left(), left->get_right()->get_right(), std::move(right),
					left->get_data(), std::forward<U>(value));

				auto retval = balance_create_middle<T>(node_colour, std::move(newLeft), std::move(newRight), left->get_right()->get_data());

				inserted = adjust_inserted<T>(inserted, left->get_right().get(), retval.get());

				return retval;
			}
		}

		if (right && colour(right) == NodeColour::Red)
		{
			if (right->get_left() && colour(right->get_left()) == NodeColour::Red)
			{
				intrusive_rb_ptr<T> newLeft, newRight;

				std::tie(newLeft, newRight) = balance_create_leftright<T>(
					std::move(left), right->get_left()->get_left(), right->get_left()->get_right(), right->get_right(),
					std::forward<U>(value), right->get_data());

				auto retval = balance_create_middle<T>(node_colour, std::move(newLeft), std::move(newRight), right->get_left()->get_data());

				inserted = adjust_inserted<T>(inserted, right->get_left().get(), retval.get());

				return retval;
			}
			else if (right->get_right() && colour(right->get_right()) == NodeColour::Red)
			{
				intrusive_rb_ptr<T> newLeft, newRight;

				std::tie(newLeft, newRight) = balance_create_leftright<T>(
					std::move(left), right->get_left(), right->get_right()->get_left(), right->get_right()->get_right(),
					std::forward<U>(value), right->get_right()->get_data());

				inserted = adjust_inserted<T>(inserted, right->get_right().get(), newRight.get());

				return balance_create_middle<T>(node_colour, std::move(newLeft), std::move(newRight), right->get_data());
			}
		}

		return make_redblack_node<T>(std::forward<U>(value), node_colour, std::move(left), std::move(right));
	}
}

WENDA_FDS_NAMESPACE_END

#endif // WENDA_FDS_DETAIL_REDBLACK_TREE_BALANCE_H_INCLUDED