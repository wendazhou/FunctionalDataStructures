#ifndef WENDA_FDS_DETAIL_REDBLACK_TREE_DATA_H_INCLUDED
#define WENDA_FDS_DETAIL_REDBLACK_TREE_DATA_H_INCLUDED

/**
* @file redblack_tree_data.h
* This file implements the principal data structures and their accessors for red black trees.
*/

#include "../FDS_common.h"
#include "../intrusive_packed_ptr.h"

WENDA_FDS_NAMESPACE_BEGIN

namespace detail
{
	/**
	* This enum represents the colour of a node in the red-black tree.
	*/
	enum class NodeColour
		: std::uint_fast32_t
	{
		Red = 0,
			Black = 1,
			DoubleBlack = 2,
			NegativeBlack = 3,
	};

	inline NodeColour operator+(NodeColour left, NodeColour right)
	{
		return NodeColour((static_cast<std::uint_fast32_t>(left) +static_cast<std::uint_fast32_t>(right)) % 4);
	}

	inline NodeColour operator-(NodeColour left, NodeColour right)
	{
		return NodeColour((static_cast<std::uint_fast32_t>(left) -static_cast<std::uint_fast32_t>(right)) % 4);
	}

	template<typename T>
	class redblack_tree_iterator;

	template<typename T>
	class redblack_node;

	template<typename T> using rb_pointer = packed_ptr<redblack_node<T>>;
	template<typename T> using const_rb_pointer = packed_ptr<const redblack_node<T>>;
	template<typename T> using intrusive_rb_ptr = intrusive_packed_ptr<redblack_node<T>>;
	template<typename T> using const_intrusive_rb_ptr = intrusive_packed_ptr<const redblack_node<T>>;

	/**
	* This class represents a node in a red-black tree.
	* @tparam T The type of the elements stored by the node.
	*/
	template<typename T>
	class redblack_node
		: public intrusive_refcount
	{
		T data; ///< The data held by the node
		const_intrusive_rb_ptr<T> left; ///< A pointer to the smaller (left) child, or null.
		const_intrusive_rb_ptr<T> right; ///< A pointer to the greater (right) child, or null.
	public:
		/**
		* Initializes a new node with the given data.
		*/
		redblack_node(T data, const_intrusive_rb_ptr<T> left,
			const_intrusive_rb_ptr<T> right)
			: data(std::move(data)), left(std::move(left)), right(std::move(right))
		{
		}

		/**
		* Returns a pointer to the minimum node from this node.
		*/
		const_rb_pointer<T> minimum() const WENDA_NOEXCEPT
		{
			return is_leaf(left) ? this : left->minimum();
		}

		/**
		* Returns a pointer to the maximum node from this node.
		*/
		const_rb_pointer<T> maximum() const WENDA_NOEXCEPT
		{
			return is_leaf(right) ? this : right->maximum();
		}

		/**
		* Finds the node with an equivalent value (in the strict weak ordering sense)
		* in the children of this node.
		* @param value The value to be found.
		* @param comp The comparison function to use. It must be compatible with the comparison
		* function used to construct this tree.
		*/
		template<typename U, typename Compare>
		const_rb_pointer<T> find(U&& value, Compare const& comp) const WENDA_NOEXCEPT
		{
			if (comp(value, data))
			{
				return is_leaf(left) ? make_null_redblack_node<T>() : left->find(std::forward<U>(value), comp);
			}
			else if (comp(data, value))
			{
				return is_leaf(right) ? make_null_redblack_node<T>() : right->find(std::forward<U>(value), comp);
			}
			else
			{
				return this;
			}
		}

		T const& get_data() const { return data; }
		const_intrusive_rb_ptr<T> const& get_left() const { return left; }
		const_intrusive_rb_ptr<T> const& get_right() const { return right; }
	};

	/**
	* Returns a pointer to a new node with the given data, colour, and left and right child.
	* @param data The data to be stored in the node.
	* @param colour The colour to be given to the returned pointer.
	* @param left The left child of the node.
	* @param right The right child of the node.
	* @returns A smart pointer to a newly created node with the given data.
	*/
	template<typename T, typename U>
	intrusive_rb_ptr<T> make_redblack_node(U&& data, NodeColour colour = NodeColour::Red,
		const_intrusive_rb_ptr<T> left = make_null_redblack_node<T>(), const_intrusive_rb_ptr<T> right = make_null_redblack_node<T>())
	{
		rb_pointer<T> rb = new redblack_node<T>(std::forward<U>(data), std::move(left), std::move(right));
		rb.set_value(static_cast<std::uint_fast32_t>(colour));
		return rb;
	}

	template<typename T>
	const_rb_pointer<T> make_null_redblack_node(NodeColour colour = NodeColour::Black) WENDA_NOEXCEPT
	{
		const_rb_pointer<T> ptr(nullptr);
		ptr.set_value(static_cast<std::uint_fast32_t>(colour));
		return ptr;
	}

	/**
	* Returns a value indicating whether there is any data stored in the node.
	* @param The node to test.
	* @returns True if the node is a leaf, that is, there is no data. Otherwise false.
	*/
	template<typename T>
	bool is_leaf(const_rb_pointer<T> node) WENDA_NOEXCEPT
	{
		if (!node)
		{
			return true;
		}

		return false;
	}

	template<typename T>
	bool is_leaf(const_intrusive_rb_ptr<T> const& node) WENDA_NOEXCEPT
	{
		return is_leaf(node.get());
	}

	template<typename T>
	bool is_leaf(intrusive_rb_ptr<T> const& node) WENDA_NOEXCEPT
	{
		return is_leaf(node.get());
	}

	/**
	* Gets the colour associated to the given node pointer.
	* @param node The pointer for which to get the colour. Can be null.
	*/
	template<typename T>
	NodeColour colour(const_rb_pointer<T> node) WENDA_NOEXCEPT
	{
		return static_cast<NodeColour>(node.get_value());
	}

	/**
	* Gets the colour associated to the given node pointer.
	* @param node The pointer for which to get the colour. Can be null.
	*/
	template<typename T>
	NodeColour colour(intrusive_rb_ptr<T> const& node) WENDA_NOEXCEPT
	{
		return colour(node.get());
	}

	/**
	* Gets the colour associated to the given node pointer.
	* @param node The pointer for which to get the colour. Can be null.
	*/
	template<typename T>
	NodeColour colour(const_intrusive_rb_ptr<T> const& node) WENDA_NOEXCEPT
	{
		return colour(node.get());
	}

	/**
	* Sets the pointer colour to the given @p colour.
	* @param node A reference to the node for which to set the colour.
	* @param colour The colour to be set.
	*/
	template<typename T>
	void set_colour(const_rb_pointer<T>& node, NodeColour colour)
	{
		node.set_value(static_cast <std::uint_fast32_t>(colour));
	}

	template<typename T>
	void set_colour(intrusive_rb_ptr<T>& node, NodeColour colour)
	{
		node.set_value(static_cast <std::uint_fast32_t>(colour));
	}

	template<typename T>
	void set_colour(const_intrusive_rb_ptr<T>& node, NodeColour colour)
	{
		node.set_value(static_cast <std::uint_fast32_t>(colour));
	}

	/**
	* Constructs a new black node from the given node, or if it is already black, returns the node.
	* @param pointer A pointer to the node for which to change the colour.
	* @returns A pointer to the same node, but coloured black.
	*/
	template<typename T>
	const_intrusive_rb_ptr<T> make_black(const_rb_pointer<T> pointer)
	{
		set_colour(pointer, NodeColour::Black);
		return pointer;
	}

	template<typename T>
	const_intrusive_rb_ptr<T> make_black(const_intrusive_rb_ptr<T> pointer)
	{
		set_colour(pointer, NodeColour::Black);
		return pointer;
	}

	/**
	* Constructs a new red node from the given node.
	* @param A pointer to the node for which to change the colour.
	* @returns A pointer tothe same node, but coloured red.
	*/
	template<typename T>
	const_intrusive_rb_ptr<T> make_red(const_rb_pointer<T> pointer)
	{
		set_colour(pointer, NodeColour::Red);
		return pointer;
	}
}

WENDA_FDS_NAMESPACE_END

#endif // WENDA_FDS_DETAIL_REDBLACK_TREE_DATA_H_INCLUDED