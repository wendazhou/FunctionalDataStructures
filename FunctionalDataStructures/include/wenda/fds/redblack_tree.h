#ifndef WENDA_FDS_REDBLACK_TREE_H_INCLUDED
#define WENDA_FDS_REDBLACK_TREE_H_INCLUDED

#include "FDS_common.h"

#include <functional>
#include <utility>
#include <iterator>
#include <tuple>

#include "intrusive_ptr.h"

/**
* @file redblack_tree.h
* This file provides the implementation for a persistent functional red-black tree.
*/

WENDA_FDS_NAMESPACE_BEGIN

namespace detail
{
	/**
    * This enum represents the colour of a node in the red-black tree.
	*/
	enum class NodeColour
	{
        NegativeBlack = -1,
        Red = 0,
        Black = 1,
        DoubleBlack = 2
	};
    
	inline NodeColour operator+(NodeColour left, NodeColour right)
	{
		return NodeColour(static_cast<int>(left) + static_cast<int>(right));
	}

	inline NodeColour operator-(NodeColour left, NodeColour right)
	{
		return NodeColour(static_cast<int>(left) - static_cast<int>(right));
	}

    template<typename T>
	class redblack_tree_iterator;

    template<typename T>
	class redblack_node;

    template<typename T, typename U, typename Compare>
	std::tuple<intrusive_ptr<const redblack_node<T>>, redblack_node<T> const*, bool> 
	insert_impl(redblack_node<T> const* tree, U&& value, Compare const& compare);

    template<typename T>
	intrusive_ptr<const redblack_node<T>> make_black(redblack_node<T> const*);

    template<typename T>
	NodeColour colour(redblack_node<T> const* node);

    template<typename T>
	NodeColour colour(intrusive_ptr<redblack_node<T>> const& node);

    template<typename T>
	NodeColour colour(intrusive_ptr<const redblack_node<T>> const& node);

	/**
    * This class represents a node in a red-black tree.
    * @tparam T The type of the elements stored by the node.
	*/
    template<typename T>
	class redblack_node
		: public intrusive_refcount
	{
		friend class redblack_tree_iterator<T>;

		template<typename T1, typename U, typename Compare>
		friend std::tuple < intrusive_ptr < const redblack_node<T1 >> , redblack_node<T1> const*, bool>
			insert_impl(redblack_node<T1> const*, U&&, Compare const&);

		template<typename T1, typename U>
		friend intrusive_ptr<redblack_node<T1>> balance(NodeColour colour, U&& value, 
			intrusive_ptr<const redblack_node<T1>> left, intrusive_ptr<const redblack_node<T1>> right,
			redblack_node<T1> const*&);

		friend intrusive_ptr<const redblack_node<T>> make_black<>(redblack_node<T> const*);
		friend NodeColour colour<>(redblack_node<T> const*);
		friend NodeColour colour<>(intrusive_ptr<redblack_node<T>> const&);
		friend NodeColour colour<>(intrusive_ptr<const redblack_node<T>> const&);

		T data; ///< The data held by the node
		NodeColour colour; ///< The colour of the node
		intrusive_ptr<const redblack_node<T>> left; ///< A pointer to the smaller (left) child, or null.
		intrusive_ptr<const redblack_node<T>> right; ///< A pointer to the greater (right) child, or null.
	public:
		/**
        * Initializes a new node with the given data.
		*/
		redblack_node(T data, NodeColour colour, intrusive_ptr<const redblack_node<T>> left, 
			intrusive_ptr<const redblack_node<T>> right)
			: data(std::move(data)), colour(std::move(colour)), left(std::move(left)), right(std::move(right))
		{
		}

		/**
        * Returns a pointer to the minimum node from this node.
		*/
		redblack_node<T> const* minimum() const WENDA_NOEXCEPT
		{
			return left == nullptr ? this : left->minimum();
		}

		/**
        * Returns a pointer to the maximum node from this node.
		*/
		redblack_node<T> const* maximum() const WENDA_NOEXCEPT
		{
			return right == nullptr ? this : right->maximum();
		}

		/**
        * Finds the node with an equivalent value (in the strict weak ordering sense)
        * in the children of this node.
        * @param value The value to be found.
        * @param comp The comparison function to use. It must be compatible with the comparison
        * function used to construct this tree.
		*/
		template<typename U, typename Compare>
		redblack_node<T> const* find(U&& value, Compare const& comp) const WENDA_NOEXCEPT
		{
			if (comp(value, data))
			{
				return left == nullptr ? nullptr : left->find(std::forward<U>(value), comp);
			}
			else if (comp(data, value))
			{
				return right == nullptr ? nullptr : right->find(std::forward<U>(value), comp);
			}
			else
			{
				return this;
			}
		}

		/**
        * Sets this node to a given colour.
		*/
		void set_colour(NodeColour colour) { this->colour = colour; }
	};

    template<typename T>
	NodeColour colour(redblack_node<T> const* node)
	{
		if (node)
		{
		    return node->colour;
		}

		return NodeColour::Black;
	}

    template<typename T>
	NodeColour colour(intrusive_ptr<redblack_node<T>> const& node)
	{
		return colour(node.get());
    }

    template<typename T>
	NodeColour colour(intrusive_ptr<const redblack_node<T>> const& node)
    {
		return colour(node.get());
    }

	/**
    * Constructs a new black node from the given node, or if it is already black, returns the node.
	*/
    template<typename T>
	intrusive_ptr<const redblack_node<T>> make_black(redblack_node<T> const* pointer)
	{
		if (pointer->colour == NodeColour::Black)
		{
			return intrusive_ptr<const redblack_node<T>>(pointer);
		}

		return make_intrusive<redblack_node<T>>(pointer->data, NodeColour::Black, pointer->left, pointer->right);
	}

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
	std::tuple<intrusive_ptr<redblack_node<T>>, intrusive_ptr<redblack_node<T>>> 
	balance_create_leftright(LL&& leftLeft, LR&& leftRight, RL&& rightLeft, RR&& rightRight, 
		ValueL&& valueLeft, ValueR&& valueRight)
	{
		auto newLeft = make_intrusive<redblack_node<T>>(
			std::forward<ValueL>(valueLeft), NodeColour::Black, 
			std::forward<LL>(leftLeft), std::forward<LR>(leftRight));

		auto newRight = make_intrusive<redblack_node<T>>(
			std::forward<ValueR>(valueRight), NodeColour::Black, 
			std::forward<RL>(rightLeft), std::forward<RR>(rightRight));

	    typedef std::tuple<intrusive_ptr<redblack_node<T>>, intrusive_ptr<redblack_node<T>>> return_t;

		return return_t(std::move(newLeft), std::move(newRight));
	}

	/**
    * Helper function for balance(), creates a red node from the given left and right black nodes.
    * @param left The left child of the node to be created.
    * @param right The right child of the node to be created.
    * @param value The value of the node to be created.
    * @tparam T The type of the element held by the nodes.
	*/
    template<typename T, typename Left, typename Right, typename Value>
	intrusive_ptr<redblack_node<T>> balance_create_middle(Left&& left, Right&& right, Value&& value)
	{
		return make_intrusive<redblack_node<T>>(std::forward<Value>(value) , NodeColour::Red,
		    std::forward<Left>(left), std::forward<Right>(right));
	}

	/**
    * Tree balancing operation as described by Okasaki in "Red-Black trees in a functional setting".
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
	intrusive_ptr<redblack_node<T>> balance(NodeColour node_colour, U&& value, 
		intrusive_ptr<const redblack_node<T>> left, intrusive_ptr<const redblack_node<T>> right,
		redblack_node<T> const*& inserted)
	{
		if (node_colour != NodeColour::Black)
		{
			return make_intrusive<redblack_node<T>>(std::forward<U>(value), node_colour, std::move(left), std::move(right));
		}

		if (left && colour(left) == NodeColour::Red)
		{
			if (left->left && colour(left->left) == NodeColour::Red)
			{
				intrusive_ptr<redblack_node<T>> newLeft, newRight;

			    std::tie(newLeft, newRight) = balance_create_leftright<T>(
					left->left->left, left->left->right, left->right, std::move(right),
					left->left->data, std::forward<U>(value));

				if (inserted == left->left.get())
				{
					inserted = newLeft.get();
				}

				return balance_create_middle<T>(std::move(newLeft), std::move(newRight), left->data);
			}
			else if (left->right && colour(left->right) == NodeColour::Red)
			{
				intrusive_ptr<redblack_node<T>> newLeft, newRight;

				std::tie(newLeft, newRight) = balance_create_leftright<T>(
					left->left, left->right->left, left->right->right, std::move(right),
					left->data, std::forward<U>(value));

				auto retval = balance_create_middle<T>(std::move(newLeft), std::move(newRight), left->right->data);

				if (inserted == left->right.get())
				{
					inserted = retval.get();
				}

				return retval;
			}
		}

		if (right && colour(right) == NodeColour::Red)
		{
			if (right->left && colour(right->left) == NodeColour::Red)
			{
				intrusive_ptr<redblack_node<T>> newLeft, newRight;

				std::tie(newLeft, newRight) = balance_create_leftright<T>(
					std::move(left), right->left->left, right->left->right, right->right,
					std::forward<U>(value), right->data);

				auto retval = balance_create_middle<T>(std::move(newLeft), std::move(newRight), right->left->data);

				if (inserted == right->left.get())
				{
					inserted = retval.get();
				}

				return retval;
			}
			else if (right->right && colour(right->right) == NodeColour::Red)
			{
				intrusive_ptr<redblack_node<T>> newLeft, newRight;

				std::tie(newLeft, newRight) = balance_create_leftright<T>(
					std::move(left), right->left, right->right->left, right->right->right,
					std::forward<U>(value), right->right->data);

				if (inserted == right->right.get())
				{
					inserted = newRight.get();
				}

				return balance_create_middle<T>(std::move(newLeft), std::move(newRight), right->data);
			}
		}

		return make_intrusive<redblack_node<T>>(std::forward<U>(value), node_colour, std::move(left), std::move(right));
	}

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
	std::tuple<intrusive_ptr<const redblack_node<T>>, redblack_node<T> const*, bool> 
	insert_impl(redblack_node<T> const* tree, U&& value, Compare const& compare)
	{
		typedef std::tuple<intrusive_ptr<const redblack_node<T>>, redblack_node<T> const*, bool> return_t;
		using std::get;

		if (!tree)
		{
			intrusive_ptr<const redblack_node<T>> newTree(make_intrusive<redblack_node<T>>(std::forward<U>(value), NodeColour::Red, nullptr, nullptr));
			auto ptr = newTree.get();
			return return_t(std::move(newTree), ptr, true);
		}

		intrusive_ptr<const redblack_node<T>> newTree;
		redblack_node<T> const* iterator;
		bool inserted;

		if (compare(value, tree->data))
		{
		    std::tie(newTree, iterator, inserted) = insert_impl(tree->left.get(), std::forward<U>(value), compare);
			auto balancedTree = balance(tree->colour, tree->data, std::move(newTree), tree->right, iterator);
			return return_t(std::move(balancedTree), iterator, inserted);
		}
		else if (compare(tree->data, value))
		{
			std::tie(newTree, iterator, inserted) = insert_impl(tree->right.get(), std::forward<U>(value), compare);
			auto balancedTree = balance(tree->colour, tree->data, tree->left, std::move(newTree), iterator);
			return return_t(std::move(balancedTree), iterator, inserted);
		}
		else
		{
			return return_t(intrusive_ptr<const redblack_node<T>>(tree), tree, false);
		}
	}

    template<typename T>
	intrusive_ptr<const redblack_node<T>> remove_node(redblack_node<T> const* node)
	{
		if (node->left && node->right)
		{
            // two children case
            // remove the maximum descendant of the left child,
            // and write its value into the current node.
			auto max = node->left->maximum();

			auto removed = remove_node(max);

			auto newLeft = make_intrusive<redblack_node<T>>(node->left->data, node->left->colour, 
				node->left->value, node->left->left, removed);

			return make_intrusive<redblack_node<T>>(max->data, node->colour, newLeft, node->right);
		}
		else if (node->left || node->right)
		{
			auto child = node->left ? node->left : node->right;
			return make_intrusive<redblack_node<T>>(child->data, NodeColour::Black, nullptr, nullptr);
		}
		else
		{
			return nullptr;
		}
	}

    template<typename T, typename U, typename Compare>
	std::tuple<intrusive_ptr<const redblack_node<T>>, redblack_node<T> const*, bool> 
	erase(redblack_node<T> const* tree, U&& value, Compare const& compare)
	{
		if (!tree)
		{
			return std::make_tuple(nullptr, nullptr, false);
		}

		intrusive_ptr<const redblack_node<T>> newTree;
		redblack_node<T> const* iterator;
		bool deleted;

		if (compare(value, tree->data))
		{
			std::tie(newTree, iterator, deleted) = erase(tree->left.get(), std::forward<U>(value), compare);
		}
		else if (compare(tree->data, value))
		{
			std::tie(newTree, iterator, deleted) = erase(tree->right.get(), std::forward<U>(value), compare);
		}
		else
		{
			deleted = true;
            
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
		redblack_node<T> const* current;
	public:
		redblack_tree_iterator() = default;
		explicit redblack_tree_iterator(redblack_node<T> const* node)
			: current(node)
		{}

		T const& operator*() const WENDA_NOEXCEPT
		{
			return current->data;
		}

		T const* operator->() const WENDA_NOEXCEPT
		{
			return std::addressof(current->data);
		}

		bool operator==(redblack_tree_iterator<T> const& other) WENDA_NOEXCEPT
		{
			return current == other.current;
		}

		bool operator!=(redblack_tree_iterator<T> const& other) WENDA_NOEXCEPT
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
	intrusive_ptr<const detail::redblack_node<T>> root; ///< The root of the tree

	explicit redblack_tree(intrusive_ptr<const detail::redblack_node<T>> const& root)
		: root(root)
    {}

	explicit redblack_tree(intrusive_ptr<const detail::redblack_node<T>>&& root)
		: root(std::move(root))
    {}
public:
	/**
    * Default constructor for the @ref redblack_tree.
    * Initializes a new empty tree.
	*/
	redblack_tree() WENDA_NOEXCEPT
		: root(nullptr)
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
			return iterator(nullptr);
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
		return iterator(nullptr);
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
		return root == nullptr;
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

		intrusive_ptr<const detail::redblack_node<T>> newRoot;
		detail::redblack_node<T> const* element;
		bool inserted;

		std::tie(newRoot, element, inserted) = detail::insert_impl(root.get(), std::forward<U>(value), Compare());

		intrusive_ptr<const detail::redblack_node<T>> blackened;

		if (inserted && element == newRoot.get())
		{
            // if we have just inserted a node at the root, we can directly change it.
			const_cast<detail::redblack_node<T>*>(newRoot.get())->set_colour(detail::NodeColour::Black);
			blackened = std::move(newRoot);
		}
		else
		{
		    blackened = detail::make_black(newRoot.get());
		}

		return return_t(redblack_tree<T>(std::move(blackened)), iterator(element), inserted);
	}
};

WENDA_FDS_NAMESPACE_END

#endif // WENDA_FDS_REDBLACK_TREE_H_INCLUDED