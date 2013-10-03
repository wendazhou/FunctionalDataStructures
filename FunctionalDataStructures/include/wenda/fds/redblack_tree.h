#ifndef WENDA_FDS_REDBLACK_TREE_H_INCLUDED
#define WENDA_FDS_REDBLACK_TREE_H_INCLUDED

#include "FDS_common.h"

#include <functional>
#include <utility>
#include <iterator>
#include <tuple>
#include <cstdint>
#include <cassert>

#include "intrusive_ptr.h"
#include "intrusive_packed_ptr.h"

#include "detail/redblack_tree_data.h"
#include "detail/redblack_tree_balance.h"
#include "detail/redblack_tree_delete.h"

/**
* @file redblack_tree.h
* This file provides the implementation for a persistent functional red-black tree.
*/

WENDA_FDS_NAMESPACE_BEGIN

namespace detail
{
	/**
	* Implementation for the insertion algorithm for the red-black trees.
	* This inserts the node at the correct position, and then rebalances the tree.
	* @param tree The node into which to insert the value.
	* @param value The value to be inserted.
	* @param compare The comparison function to be used to define the insertion position.
	* It must be compatible with the ordering of the tree.
	* @returns A tuple containing a pointer to the new tree, a pointer to the inserted node, and a
	* boolean indicating whether anything was inserted.
	*/
	template<typename T, typename U, typename Compare>
	std::tuple<const_intrusive_rb_ptr<T>, const_rb_pointer<T>, bool>
		insert_impl(const_rb_pointer<T> tree, U&& value, Compare const& compare)
	{
		typedef std::tuple<const_intrusive_rb_ptr<T>, const_rb_pointer<T>, bool> return_t;
		using std::get;

		if (!tree)
		{
			const_intrusive_rb_ptr<T> newTree(make_redblack_node<T>(std::forward<U>(value),
				NodeColour::Red, make_null_redblack_node<T>(), make_null_redblack_node<T>()));
			auto ptr = newTree.get();
			return return_t(std::move(newTree), ptr, true);
		}

		const_intrusive_rb_ptr<T> newTree;
		const_rb_pointer<T> iterator;
		bool inserted;

		if (compare(value, tree->get_data()))
		{
			std::tie(newTree, iterator, inserted) = insert_impl(tree->get_left().get(), std::forward<U>(value), compare);
			auto balancedTree = balance(colour(tree), tree->get_data(), std::move(newTree), tree->get_right(), iterator);
			return return_t(std::move(balancedTree), iterator, inserted);
		}
		else if (compare(tree->get_data(), value))
		{
			std::tie(newTree, iterator, inserted) = insert_impl(tree->get_right().get(), std::forward<U>(value), compare);
			auto balancedTree = balance(colour(tree), tree->get_data(), tree->get_left(), std::move(newTree), iterator);
			return return_t(std::move(balancedTree), iterator, inserted);
		}
		else
		{
			return return_t(const_intrusive_rb_ptr<T>(tree), tree, false);
		}
		}


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
	public:
		redblack_tree_iterator() = default;
		explicit redblack_tree_iterator(const_rb_pointer<T> node)
			: current(node)
		{}

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
	};
}

/**
* This class implements a functional red-black tree.
* @tparam T The type of the elements stored in the tree.
* @tparam Compare The comparison function to be used in the tree.
*/
template<typename T, typename Compare = std::less<> >
class redblack_tree
{
public:
	/**
    * This typedef represents the type of the elements stored in the tree.
	*/
	typedef T value_type;
	/**
    * This typedef represents the type of the iterator.
	*/
	typedef detail::redblack_tree_iterator<T> iterator;
private:
	detail::const_intrusive_rb_ptr<T> root; ///< The root of the tree

	explicit redblack_tree(detail::const_intrusive_rb_ptr<T> const& root)
		: root(root)
    {}

	explicit redblack_tree(detail::const_intrusive_rb_ptr<T>&& root)
		: root(std::move(root))
    {}
public:
	/**
    * Default constructor for the @ref redblack_tree.
    * Initializes a new empty tree.
	*/
	redblack_tree() WENDA_NOEXCEPT
		: root(detail::make_null_redblack_node<T>())
	{}

	/**
    * Copy constructor for @ref redblack_tree.
	*/
	redblack_tree(redblack_tree<T> const& other)
		: root(other.root)
	{}

	/**
    * Move constructor for @ref redblack_tree.
	*/
	redblack_tree(redblack_tree<T>&& other) WENDA_NOEXCEPT
		: root(std::move(other.root))
	{}

	/**
    * Finds the given value in the red-black tree, returning
    * an iterator to the value if it is found, if the element is
    * not found, returns an iterator pointing to end().
    * We say a value is found if there exists an equivalent value
    * in the red-black tree with respect to the strict weak ordering
    * determined by Compare.
    * @param value The value to find.
    * @returns An iterator to the equivalent element if found; otherwise
    * an iterator to end().
	*/
	iterator find(T const& value) const WENDA_NOEXCEPT
	{
		if (root)
		{
			return iterator(root->find(value, Compare()));
		}

		return end();
	}

	/**
    * Returns an iterator to the first and smallest element in the tree.
    * @returns An iterator to the smallest element in the tree.
	*/
	iterator begin() const WENDA_NOEXCEPT
	{
		if (root)
		{
		    return iterator(root->minimum());
		}
		else
		{
			return iterator(detail::make_null_redblack_node<T>());
		}
	}

	/**
    * Returns an iterator to the first and smallest element in the tree.
    * @returns An iterator to the smallest element in the tree.
	*/
	iterator cbegin() const WENDA_NOEXCEPT
	{
		return begin();
	}

	/**
    * Returns an iterator one past the last (largest) element in the tree.
    * @returns An iterator to the largest element in the tree.
	*/
	iterator end() const WENDA_NOEXCEPT
	{
		return iterator(detail::make_null_redblack_node<T>());
	}

	/**
    * Returns an iterator one past the last (largest) element in the tree.
    * @returns An iterator to the largest element in the tree.
	*/
	iterator cend() const WENDA_NOEXCEPT
	{
		return end();
	}

	/**
    * Tests whether there are any elements in the tree.
    * @returns True if the tree is empty; otherwise false.
	*/
	bool empty() const WENDA_NOEXCEPT
	{
		if (!root)
		{
			return true;
		}

		return false;
	}

	/**
    * Returns a new tree containing the given value.
    * This inserts the element into a new tree if it does not already exist, 
	* not modifying this instance of the tree. If an equivalent value already exists,
    * in the sense of the strict weak ordering induced by Compare, it does not create
    * a new tree, but simply returns the existing tree.
    * @param value The value to be inserted.
    * @returns A tuple of three values, containing:
    * - first, a pointer to the new tree
    * - second, an iterator to the newly inserted element, if it was inserted, or an equivalent value, if it already existed.
    * - third, a boolean value that is true if a value has been inserted, and false if the value was already present.
	*/
    template<typename U>
	std::tuple<redblack_tree<T>, iterator, bool> insert(U&& value) const
	{
		typedef std::tuple<redblack_tree<T>, iterator, bool> return_t;

		detail::const_intrusive_rb_ptr<T> newRoot;
		detail::const_rb_pointer<T> element;
		bool inserted;

		std::tie(newRoot, element, inserted) = detail::insert_impl(root.get(), std::forward<U>(value), Compare());

		detail::const_intrusive_rb_ptr<T> blackened;

		blackened = detail::make_black(newRoot.get());

		return return_t(redblack_tree<T>(std::move(blackened)), iterator(element), inserted);
	}

	template<typename U>
	std::tuple<redblack_tree<T>, bool> erase(U&& value) const
	{
		typedef  std::tuple<redblack_tree<T>, bool> return_t;

		detail::const_intrusive_rb_ptr<T> newRoot;
		bool deleted;

		std::tie(newRoot, deleted) = detail::find_delete_node(root.get(), std::forward<U>(value), Compare());

		auto blackened = detail::make_black(newRoot);

		return return_t(redblack_tree<T>(std::move(blackened)), deleted);
	}
};

WENDA_FDS_NAMESPACE_END

#endif // WENDA_FDS_REDBLACK_TREE_H_INCLUDED