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
	};

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

    template<typename T>
	class forward_list_iterator
		: std::iterator<std::forward_iterator_tag, T, std::ptrdiff_t, T const*, T const&>
	{
		forward_list_next<T> const* current;
	public:
		forward_list_iterator() WENDA_NOEXCEPT
			: current(nullptr)
		{
		}

		explicit forward_list_iterator(forward_list_next<T> const* current) WENDA_NOEXCEPT
			: current(current)
		{}

		reference operator*() const WENDA_NOEXCEPT
		{
			return static_cast<forward_list_node<T> const*>(current)->value;
		}

		pointer operator->() const WENDA_NOEXCEPT
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
	forward_list() WENDA_NOEXCEPT
		: forward_list_next(nullptr)
	{
	}

	forward_list(forward_list<T> const& other)
		: forward_list_next(other)
	{}

	forward_list(forward_list<T>&& other) WENDA_NOEXCEPT
		: forward_list_next(std::move(other.next))
	{}

	typedef T value_type;
	typedef detail::forward_list_iterator<T> iterator;

	forward_list<T> push_front(T const& value) const
	{
		return forward_list<T>(make_intrusive<detail::forward_list_node<T>> (value, next));
	}

	forward_list<T> push_front(T&& value) const
	{
		return forward_list<T>(make_intrusive<detail::forward_list_node<T>>(std::move(value), next));
	}

    template<typename... Args>
	forward_list<T> emplace_front(Args... args) const
	{
		return forward_list<T>(make_intrusive<detail::forward_list_node<T>>(T(std::forward<Args>(args)...), next));
	}

	T const& front() const WENDA_NOEXCEPT
	{
		return next->value;
	}

	bool empty() const WENDA_NOEXCEPT
	{
		return next == nullptr;
	}

	iterator before_begin() const WENDA_NOEXCEPT
	{
		return iterator(this);
	}

	iterator cbefore_begin() const WENDA_NOEXCEPT
	{
		return iterator(this);
	}

	iterator begin() const WENDA_NOEXCEPT
	{
		return iterator(next.get());
	}

	iterator cbegin() const WENDA_NOEXCEPT
	{
		return iterator(next.get());
	}

	iterator end() const WENDA_NOEXCEPT
	{
		return iterator(nullptr);
	}

	iterator cend() const WENDA_NOEXCEPT
	{
		return iterator(nullptr);
	}
};

WENDA_FDS_NAMESPACE_END


#endif // WENDA_FDS_FORWARD_LIST_H_INCLUDED