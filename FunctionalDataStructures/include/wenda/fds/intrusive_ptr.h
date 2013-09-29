#ifndef WENDA_FDS_INTRUSIVE_PTR_H_INCLUDED
#define WENDA_FDS_INTRUSIVE_PTR_H_INCLUDED

#include "FDS_common.h"
#include <type_traits>
#include <atomic>
#include <cstddef>

WENDA_FDS_NAMESPACE_BEGIN

template<typename T>
class intrusive_ptr
{
	T* pointer;
public:
	intrusive_ptr(T* pointer)
		: pointer(pointer)
	{
		if (pointer)
		{
		    add_reference(pointer);
		}
	}

	intrusive_ptr() WENDA_NOEXCEPT
		: pointer(nullptr)
	{
	}

	intrusive_ptr(std::nullptr_t) WENDA_NOEXCEPT
		: pointer(nullptr)
	{
	}

	intrusive_ptr(intrusive_ptr<T> const& other)
		: pointer(other.pointer)
	{
		if (pointer)
		{
			add_reference(pointer);
		}
	}

	intrusive_ptr(intrusive_ptr<T>&& other) WENDA_NOEXCEPT
		: pointer(other.pointer)
	{
		other.pointer = nullptr;
	}

    template<typename U, typename SFINAE = typename std::enable_if<std::is_assignable<T*, U*>::value, void>::type>
	intrusive_ptr(intrusive_ptr<U> const& other)
		: pointer(other.pointer)
	{
		if (pointer)
		{
		    add_reference(pointer);
		}
	}

    template<typename U, typename SFINAE = typename std::enable_if<std::is_assignable<T*, U*>::value, void>::type>
	intrusive_ptr(intrusive_ptr<U>&& other) WENDA_NOEXCEPT
		: pointer(other.pointer)
	{
		other.pointer = nullptr;
	}

	~intrusive_ptr()
	{
		if (pointer)
		{
		    auto refCount = remove_reference(pointer);
		    if (refCount == 0)
		    {
		    	delete pointer;
		    }
		}
	}

	T* get() WENDA_NOEXCEPT { return pointer; }
	T const* get() const WENDA_NOEXCEPT { return pointer; }

	T& operator*() WENDA_NOEXCEPT { return *pointer; }
	T const& operator*() const WENDA_NOEXCEPT { return *pointer; }

	T* operator->() WENDA_NOEXCEPT { return pointer; }
	T const* operator->() const WENDA_NOEXCEPT { return pointer; }

	explicit operator bool() const WENDA_NOEXCEPT
	{
		return pointer;
	}
};

template<typename T, typename U>
bool operator==(intrusive_ptr<T> const& left, intrusive_ptr<T> const& right) WENDA_NOEXCEPT
{
	return left.get() == right.get();
}

template<typename T>
bool operator==(intrusive_ptr<T> const& left, std::nullptr_t) WENDA_NOEXCEPT
{
	return left.get() == nullptr;
}

template<typename T>
bool operator==(std::nullptr_t, intrusive_ptr<T> const& right) WENDA_NOEXCEPT
{
	return nullptr == right.get();
}

template<typename T, typename U>
bool operator!=(intrusive_ptr<T> const& left, intrusive_ptr<T> const& right) WENDA_NOEXCEPT
{
	return left.get() != right.get();
}

template<typename T>
bool operator!=(intrusive_ptr<T> const& left, std::nullptr_t) WENDA_NOEXCEPT
{
	return left.get() != nullptr;
}

template<typename T>
bool operator!=(std::nullptr_t, intrusive_ptr<T> const& right) WENDA_NOEXCEPT
{
	return nullptr != right.get();
}

template<typename T, typename... Args>
intrusive_ptr<T> make_intrusive(Args... args)
{
	return intrusive_ptr<T>(new T(std::forward<Args>(args)...));
}

class intrusive_refcount
{
	friend std::size_t add_reference(intrusive_refcount*);
    friend std::size_t remove_reference(intrusive_refcount* obj);

	std::atomic_size_t counter;
public:
	intrusive_refcount() WENDA_NOEXCEPT
		: counter(0)
	{
	}

	intrusive_refcount(intrusive_refcount const& other) WENDA_NOEXCEPT
		: counter(0)
	{
	}
};

std::size_t add_reference(intrusive_refcount* obj) WENDA_NOEXCEPT
{
    return ++(obj->counter);
}

std::size_t remove_reference(intrusive_refcount* obj) WENDA_NOEXCEPT
{
	return --(obj->counter);
}

WENDA_FDS_NAMESPACE_END

#endif //WENDA_FDS_INTRUSIVE_PTR_H_INCLUDED