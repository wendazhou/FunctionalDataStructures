#ifndef WENDA_FDS_REDBLACK_TREE_H_INCLUDED
#define WENDA_FDS_REDBLACK_TREE_H_INCLUDED

#include "FDS_common.h"

#include <functional>
#include <utility>
#include <iterator>
#include <tuple>

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
	class redblack_node;

    template<typename T, typename U, typename Compare>
	std::tuple<intrusive_ptr<const redblack_node<T>>, redblack_node<T> const*, bool> 
	insert_impl(redblack_node<T> const* tree, U&& value, Compare const& compare);

    template<typename T>
	intrusive_ptr<const redblack_node<T>> make_black(redblack_node<T> const*);

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

		T data;
		NodeColour colour;
		intrusive_ptr<const redblack_node<T>> left;
		intrusive_ptr<const redblack_node<T>> right;
	public:
		redblack_node(T data, NodeColour colour, intrusive_ptr < const redblack_node < T >> left, intrusive_ptr < const redblack_node < T >> right)
			: data(std::move(data)), colour(std::move(colour)),
			left(std::move(left)), right(std::move(right))
		{
		}

		redblack_node<T> const* minimum() const WENDA_NOEXCEPT
		{
			return left == nullptr ? this : left->minimum();
		}

		redblack_node<T> const* maximum() const WENDA_NOEXCEPT
		{
			return right == nullptr ? this : right->maximum();
		}

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

		void set_colour(NodeColour colour) { this->colour = colour; }
		NodeColour get_colour() const { return this->colour; }
	};

    template<typename T>
	intrusive_ptr<const redblack_node<T>> make_black(redblack_node<T> const* pointer)
	{
		if (pointer->colour == NodeColour::Black)
		{
			return intrusive_ptr<const redblack_node<T>>(pointer);
		}

		return make_intrusive<redblack_node<T>>(pointer->data, NodeColour::Black, pointer->left, pointer->right);
	}

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

    template<typename T, typename Left, typename Right, typename Value>
	intrusive_ptr<redblack_node<T>> balance_create_middle(Left&& left, Right&& right, Value&& value)
	{
		return make_intrusive<redblack_node<T>>(std::forward<Value>(value) , NodeColour::Red,
		    std::forward<Left>(left), std::forward<Right>(right));
	}

    template<typename T, typename U>
	intrusive_ptr<redblack_node<T>> balance(NodeColour colour, U&& value, 
		intrusive_ptr<const redblack_node<T>> left, intrusive_ptr<const redblack_node<T>> right,
		redblack_node<T> const*& inserted)
	{
		if (colour != NodeColour::Black)
		{
			return make_intrusive<redblack_node<T>>(std::forward<U>(value), colour, std::move(left), std::move(right));
		}

		if (left && left->colour == NodeColour::Red)
		{
			if (left->left && left->left->colour == NodeColour::Red)
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
			else if (left->right && left->right->colour == NodeColour::Red)
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

		if (right && right->colour == NodeColour::Red)
		{
			if (right->left && right->left->colour == NodeColour::Red)
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
			else if (right->right && right->right->colour == NodeColour::Red)
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

		return make_intrusive<redblack_node<T>>(std::forward<U>(value), colour, std::move(left), std::move(right));
	}

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

template<typename T>
struct is_transparent
	: std::false_type
{};

template<>
struct is_transparent<std::less<>>
	: std::true_type
{};

template<typename T, typename Compare = std::less<> >
class redblack_tree
{
public:
	typedef T value_type;
	typedef detail::redblack_tree_iterator<T> iterator;
private:
	intrusive_ptr<const detail::redblack_node<T>> root;

	explicit redblack_tree(intrusive_ptr<const detail::redblack_node<T>> const& root)
		: root(root)
    {}

	explicit redblack_tree(intrusive_ptr<const detail::redblack_node<T>>&& root)
		: root(std::move(root))
    {}
public:
	redblack_tree() WENDA_NOEXCEPT
		: root(nullptr)
	{}

	redblack_tree(redblack_tree<T> const& other)
		: root(other.root)
	{}

	redblack_tree(redblack_tree<T>&& other) WENDA_NOEXCEPT
		: root(std::move(other.root))
	{}

	iterator find(T const& value) const WENDA_NOEXCEPT
	{
		if (root)
		{
			return iterator(root->find(value, Compare()));
		}

		return end();
	}

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

	iterator cbegin() const WENDA_NOEXCEPT
	{
		return begin();
	}

	iterator end() const WENDA_NOEXCEPT
	{
		return iterator(nullptr);
	}

	iterator cend() const WENDA_NOEXCEPT
	{
		return end();
	}

	bool empty() const WENDA_NOEXCEPT
	{
		return root == nullptr;
	}

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

	iterator find(T const& value)
	{
		if (root)
		{
			return iterator(root->find(value, Compare()));
		}

		return iterator(nullptr);
	}
};

WENDA_FDS_NAMESPACE_END

#endif // WENDA_FDS_REDBLACK_TREE_H_INCLUDED