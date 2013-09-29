#ifndef WENDA_FDS_LIST_H_INCLUDED
#define WENDA_FDS_LIST_H_INCLUDED

#include "FDS_common.h"

#include <utility>

#include "intrusive_ptr.h"

WENDA_FDS_NAMESPACE_BEGIN

namespace detail
{
    template<typename T>
	struct ListNode
		: intrusive_refcount
	{
		T value;
		intrusive_ptr<ListNode<T>> next;

		ListNode(T value, intrusive_ptr<ListNode<T>> next)
			: value(std::move(value)), next(std::move(next))
		{
		}
	};
}

template<typename T>
class List
{
	intrusive_ptr<detail::ListNode<T>> next;

	List(intrusive_ptr < detail::ListNode < T >> const& next)
		: next(next)
	{
	}

	List(intrusive_ptr < detail::ListNode < T >> &&next)
		: next(std::move(next))
	{
	}
public:
	List()
		: next(nullptr)
	{
	}

	List(List<T> const& other)
		: next(other.next)
	{}

	List(List<T>&& other)
		: next(std::move(other.next))
	{}

	List<T> push_front(T const& value) const
	{
		return List<T>(make_intrusive<detail::ListNode<T>> (value, next));
	}

	List<T> push_front(T&& value) const
	{
		return List<T>(make_intrusive<detail::ListNode<T>>(std::move(value), next));
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


#endif // WENDA_FDS_LIST_H_INCLUDED