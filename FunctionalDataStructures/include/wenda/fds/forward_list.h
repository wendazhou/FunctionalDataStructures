#ifndef WENDA_FDS_FORWARD_LIST_H_INCLUDED
#define WENDA_FDS_FORWARD_LIST_H_INCLUDED

#include "FDS_common.h"

#include <utility>

#include "intrusive_ptr.h"

WENDA_FDS_NAMESPACE_BEGIN

namespace detail
{
    template<typename T>
	struct forward_list_node
		: intrusive_refcount
	{
		T value;
		intrusive_ptr<forward_list_node<T>> next;

		forward_list_node(T value, intrusive_ptr<forward_list_node<T>> next)
			: value(std::move(value)), next(std::move(next))
		{
		}
	};
}

template<typename T>
class forward_list
{
	intrusive_ptr<detail::forward_list_node<T>> next;

	forward_list(intrusive_ptr<detail::forward_list_node<T>> const& next)
		: next(next)
	{
	}

	forward_list(intrusive_ptr<detail::forward_list_node<T>> &&next)
		: next(std::move(next))
	{
	}
public:
	forward_list()
		: next(nullptr)
	{
	}

	forward_list(forward_list<T> const& other)
		: next(other.next)
	{}

	forward_list(forward_list<T>&& other)
		: next(std::move(other.next))
	{}

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

	T const& front() const
	{
		return next->value;
	}

	bool empty() const
	{
		return next == nullptr;
	}
};

WENDA_FDS_NAMESPACE_END


#endif // WENDA_FDS_FORWARD_LIST_H_INCLUDED