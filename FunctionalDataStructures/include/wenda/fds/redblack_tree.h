#ifndef WENDA_FDS_REDBLACK_TREE_H_INCLUDED
#define WENDA_FDS_REDBLACK_TREE_H_INCLUDED

#include "FDS_common.h"

#include <functional>
#include <utility>
#include <iterator>

#include "intrusive_ptr.h"

WENDA_FDS_NAMESPACE_BEGIN

namespace detail
{
	enum class NodeColour
	{
        Black,
        Red,
        DoubleBlack
	};

    template<typename T>
	class redblack_tree_iterator;

    template<typename T>
	class redblack_node
		: public intrusive_refcount
	{
		friend class redblack_tree_iterator<T>;

		T data;
		NodeColour colour;
		redblack_node<T>* parent;
		intrusive_ptr<redblack_node<T>> left;
		intrusive_ptr<redblack_node<T>> right;
	public:
		bool is_left_child() const WENDA_NOEXCEPT
		{
			return (parent != nullptr) && (parent->left == this);
		}

		bool is_right_child() const WENDA_NOEXCEPT
		{
			return (parent != nullptr) && (parent->right == this);
		}

		redblack_node<T> const* minimum() const WENDA_NOEXCEPT
		{
			return left == nullptr ? this : left->minimum();
		}

		redblack_node<T> const* maximum() const WENDA_NOEXCEPT
		{
			return right == nullptr ? this : right->maximum();
		}

		redblack_node<T> const* successor() const WENDA_NOEXCEPT
		{
			if (right != null)
			{
				return right->minimum();
			}

			if (is_left_child())
			{
				return parent;
			}

			auto next = this;

			do
			{
				next = next->parent;
			} while ((next != nullptr) && next->is_right_child());

			return next == nullptr ? nullptr : next->parent;
		}
	};

    template<typename T>
	class redblack_tree_iterator
		: public std::iterator<std::bidirectional_iterator_tag, T, std::ptrdiff_t, T const*, T const&>
	{
		redblack_node<T> const* current;
	public:
		redblack_tree_iterator() = default;
		redblack_tree_iterator(redblack_node<T> const* node)
			: current(node)
		{}

		redblack_tree_iterator<T>& operator++() WENDA_NOEXCEPT
		{
			current = current->successor();
		}

		redblack_tree_iterator operator++(int)
		{
			auto result = *this;
			current = current->successor();
			return result;
		}

		T const& operator*() const WENDA_NOEXCEPT
		{
			return current->data;
		}

		T const* operator->() const WENDA_NOEXCEPT
		{
			return std::addressof(current->data);
		}

		bool operator==(redblack_tree_iterator<T> const& other)
		{
			return current == other.current;
		}

		bool operator!=(redblack_tree_iterator<T> const& other)
		{
			return current != other.current;
		}
	};
}

template<typename T, typename Compare = std::less<> >
class redblack_tree
{
	intrusive_ptr<detail::redblack_node<T>> root;
public:
	typedef T value_type;
	typedef detail::redblack_tree_iterator<T> iterator;

	redblack_tree()
		: root(nullptr)
	{}

	redblack_tree(redblack_tree<T> const& other)
		: root(other.root)
	{}

	redblack_tree(redblack_tree<T>&& other) WENDA_NOEXCEPT
		: root(std::move(other.root))
	{}

	iterator begin() const WENDA_NOEXCEPT
	{
		return iterator(root->minimum());
	}

	iterator cbegin() const WENDA_NOEXCEPT
	{
		return begin();
	}

	iterator end() const WENDA_NOEXCEPT
	{
		return iterator(root->maximum());
	}

	iterator cend() const WENDA_NOEXCEPT
	{
		return end();
	}

	bool empty() const WENDA_NOEXCEPT
	{
		return root == nullptr;
	}
};

WENDA_FDS_NAMESPACE_END

#endif // WENDA_FDS_REDBLACK_TREE_H_INCLUDED