#ifndef WENDA_FDS_FORWARD_LIST_H_INCLUDED
#define WENDA_FDS_FORWARD_LIST_H_INCLUDED

#include "FDS_common.h"

#include <utility>
#include <iterator>

#include "intrusive_ptr.h"

WENDA_FDS_NAMESPACE_BEGIN

namespace detail
{
    template<typename T>
	struct forward_list_node;

    template<typename T>
	class forward_list_iterator;

	/**
	* This class implements a way of holding a next pointer to a @ref forward_list_node.
	*/
    template<typename T>
	class forward_list_next
	{
		friend class forward_list_iterator<T>;
	protected:
		intrusive_ptr<forward_list_node<T>> next;

		forward_list_next() WENDA_NOEXCEPT
			: next(nullptr)
		{}

		forward_list_next(intrusive_ptr<forward_list_node<T>> const& next)
			: next(next)
		{}

		forward_list_next(intrusive_ptr<forward_list_node<T>>&& next) WENDA_NOEXCEPT
			: next(std::move(next))
		{}

		forward_list_next(forward_list_next const& other)
			: next(other.next)
		{}

		forward_list_next(forward_list_next&& other) WENDA_NOEXCEPT
			: next(std::move(other.next))
		{}

		forward_list_next& operator=(forward_list_next const& other)
		{
			next = other.next;
			return *this;
		}

		forward_list_next& operator=(forward_list_next&& other)
		{
			next = std::move(other.next);
			return *this;
		}
	};

	/**
	* This class implements a forward list node.
	* @tparam T The type of the value held by the node.
	*/
    template<typename T>
	struct forward_list_node
		: intrusive_refcount, forward_list_next<T>
	{
		T value;

		forward_list_node(T value, intrusive_ptr<forward_list_node<T>> next)
			: value(std::move(value)), forward_list_next(std::move(next))
		{
		}
	};

	/**
	* This class implements an iterator for @ref forward_list_iterator.
	* This iterator models a forward iterator.
	*/
    template<typename T>
	class forward_list_iterator
		: std::iterator<std::forward_iterator_tag, T, std::ptrdiff_t, T const*, T const&>
	{
		forward_list_next<T> const* current;
	public:
		forward_list_iterator() = default;

		explicit WENDA_CONSTEXPR forward_list_iterator(forward_list_next<T> const* current) WENDA_NOEXCEPT
			: current(current)
		{}

		WENDA_CONSTEXPR reference operator*() const WENDA_NOEXCEPT
		{
			return static_cast<forward_list_node<T> const*>(current)->value;
		}

		WENDA_CONSTEXPR pointer operator->() const WENDA_NOEXCEPT
		{
			return std::addressof(static_cast<forward_list_node<T> const*>(current)->value);
		}

		forward_list_iterator& operator++() WENDA_NOEXCEPT
		{
			current = current->next.get();
			return *this;
		}

		forward_list_iterator operator++(int) WENDA_NOEXCEPT
		{
			auto r = *this;
			current = &current->next;
			return r;
		}

		bool operator==(forward_list_iterator<T> const& other) WENDA_NOEXCEPT
		{
			return current == other.current;
		}

		bool operator!=(forward_list_iterator<T> const& other) WENDA_NOEXCEPT
		{
			return current != other.current;
		}
	};
}

/**
* This class implements a standard cons list as exists in many functional
* programming languages.
* The @ref forward_list shares the common nodes when possible. 
* @tparam The type of the elements contained in the list.
*/
template<typename T>
class forward_list
	: detail::forward_list_next<T>
{
	friend class detail::forward_list_iterator<T>;

	forward_list(intrusive_ptr<detail::forward_list_node<T>> const& next)
		: forward_list_next(next)
	{
	}

	forward_list(intrusive_ptr<detail::forward_list_node<T>> &&next)
		: forward_list_next(std::move(next))
	{
	}
public:
	/**
	* Defauld constructor, constructs an empty container.
	*/
	forward_list() WENDA_NOEXCEPT
		: forward_list_next(nullptr)
	{
	}

	/**
	* Copy constructor.
	* Constructs a new @ref forward_list referencing the elements from the @p other list.
	*/
	forward_list(forward_list<T> const& other)
		: forward_list_next(other)
	{}

	/**
	* Move constructor.
	* Constructs a new @ref forward_list referencing the elements from the @p other list.
	*/
	forward_list(forward_list<T>&& other) WENDA_NOEXCEPT
		: forward_list_next(std::move(other.next))
	{}

	/**
	* Copy assignment operator.
	* Replaces the referenced list to the list referenced by @p other.
	*/
	forward_list& operator=(forward_list const& other)
	{
		forward_list_next::operator=(other);
		return *this;
	}

	/**
	* Move assignment operator.
	* Replaces the referenced list to the list referenced by @p other.
	*/
	forward_list& operator=(forward_list&& other)
	{
		forward_list_next::operator=(std::move(other));
		return *this;
	}

	typedef T value_type; ///< The type of the elements contained in the list.
	typedef detail::forward_list_iterator<T> iterator; ///< The type of the iterator used to enumerate the list.

	/**
	* Returns a new list with the given @p value prepended to the current list.
	* @param value The value to prepend to the list. It is copied into the list.
	* @returns A new list with the value prepended.
	*/
	forward_list<T> push_front(T const& value) const
	{
		return forward_list<T>(make_intrusive<detail::forward_list_node<T>> (value, next));
	}

	/**
	* Returns a new list with the given @p value prepended to the current list.
	* @param value The value to prepend to the list. It is moved into the list.
	* @returns A new list with the value prepended.
	*/
	forward_list<T> push_front(T&& value) const
	{
		return forward_list<T>(make_intrusive<detail::forward_list_node<T>>(std::move(value), next));
	}

	/**
	* Returns a new list with a prepended value that is in-place constructed from the given arguments.
	* @param args The arguments to forward to the constructor of T.
	* @returns A new list with the constructed value prepended.
	*/
    template<typename... Args>
	forward_list<T> emplace_front(Args... args) const
	{
		return forward_list<T>(make_intrusive<detail::forward_list_node<T>>(T(std::forward<Args>(args)...), next));
	}

	/**
	* Returns a reference to the first element in the list.
	* Calling front() on an empty list produces undefined behaviour.
	* @returns reference to the first element.
	*/
	T const& front() const WENDA_NOEXCEPT
	{
		return next->value;
	}

	/**
	* Returns a value indicating whether there are any elements in the list.
	* @returns True if the list is empty, otherwise false.
	*/
	bool empty() const WENDA_NOEXCEPT
	{
		return next == nullptr;
	}

	/**
	* Returns an iterator pointing one before the beginning of the list.
	*/
	iterator before_begin() const WENDA_NOEXCEPT
	{
		return iterator(this);
	}

	/**
	* Returns an iterator pointing one before the beginning of the list.
	*/
	iterator cbefore_begin() const WENDA_NOEXCEPT
	{
		return iterator(this);
	}

	/**
	* Returns an iterator to the first element of the list.
	*/
	iterator begin() const WENDA_NOEXCEPT
	{
		return iterator(next.get());
	}

	/**
	* Returns a constant iterator to the first element of the list.
	*/
	iterator cbegin() const WENDA_NOEXCEPT
	{
		return iterator(next.get());
	}

	/**
	* Returns an iterator to the end of the list.
	*/
	iterator end() const WENDA_NOEXCEPT
	{
		return iterator(nullptr);
	}

	/**
	* Returns a constant iterator to the end of the list.
	*/
	iterator cend() const WENDA_NOEXCEPT
	{
		return iterator(nullptr);
	}
};

WENDA_FDS_NAMESPACE_END


#endif // WENDA_FDS_FORWARD_LIST_H_INCLUDED