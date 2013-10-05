#ifndef WENDA_FDS_DETAIL_REDBLACK_TREE_ITERATOR_H_INCLUDED
#define WENDA_FDS_DETAIL_REDBLACK_TREE_ITERATOR_H_INCLUDED

#include "../FDS_common.h"
#include "redblack_tree_data.h"

#include <stack>

WENDA_FDS_NAMESPACE_BEGIN

namespace detail {

	/**
	* This class implements an iterator for the red-black tree.
	* The iterators for red-black trees model bidirectional iterators.
	* @tparam T The type of the elements in the tree.
	*/
	template<typename T>
	class redblack_tree_iterator
		: public std::iterator<std::bidirectional_iterator_tag, T, std::ptrdiff_t, T const*, T const&>
	{
		const_rb_pointer<T> current;
		std::stack<const_rb_pointer<T>> stack;
		bool goRight;

		void next()
		{
			if (goRight)
			{
				goRight = false;
				current = current->get_right().get();
			}

			while (current)
			{
				stack.push(current);
				current = current->get_left().get();
			}

			if (stack.empty())
			{
				current = make_null_redblack_node<T>();
			}
			else
			{
				current = stack.top();
				stack.pop();
				goRight = true;
			}
		}
	public:
		redblack_tree_iterator() = default;

		redblack_tree_iterator(redblack_tree_iterator const& other)
			: current(other.current), goRight(other.goRight), stack(other.stack)
		{}

		redblack_tree_iterator(redblack_tree_iterator&& other)
			: current(std::move(other.current)), goRight(std::move(other.goRight)), stack(std::move(other.stack))
		{}

		explicit redblack_tree_iterator(const_rb_pointer<T> node, bool traverse = false)
			: current(node), goRight(false)
		{
			if (traverse)
			{
				next();
			}
		}

		T const& operator*() const WENDA_NOEXCEPT
		{
			return current->get_data();
		}

		T const* operator->() const WENDA_NOEXCEPT
		{
			return std::addressof(current->get_data());
		}

		bool operator==(redblack_tree_iterator const& other) WENDA_NOEXCEPT
		{
			return current == other.current;
		}

		bool operator!=(redblack_tree_iterator const& other) WENDA_NOEXCEPT
		{
			return current != other.current;
		}

		redblack_tree_iterator& operator++()
		{
			next();
			return *this;
		}

		redblack_tree_iterator operator++(int)
		{
			redblack_tree_iterator _this = *this;
			next();
			return _this;
		}
	};
}

WENDA_FDS_NAMESPACE_END

#endif // WENDA_FDS_DETAIL_REDBLACK_TREE_ITERATOR_H_INCLUDED