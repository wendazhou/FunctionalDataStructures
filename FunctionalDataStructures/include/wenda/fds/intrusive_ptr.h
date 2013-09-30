#ifndef WENDA_FDS_INTRUSIVE_PTR_H_INCLUDED
#define WENDA_FDS_INTRUSIVE_PTR_H_INCLUDED

#include "FDS_common.h"
#include <type_traits>
#include <atomic>
#include <cstddef>

WENDA_FDS_NAMESPACE_BEGIN

template<typename T>
class intrusive_ptr;

/**
* This class implements an intrusive smart pointer.
* In order for a type @p T to be used with the smart pointer,
* there must exists free functions add_reference() and remove_reference()
* that can be invoked with pointers to @p T.
* @tparam T The type pointed to by this pointer. It may be an incomplete type.
*/
template<typename T>
class intrusive_ptr
{
	T* pointer;
	template<typename U> friend class intrusive_ptr;
public:
	/**
	* Initializes the @ref intrusive_ptr to a default value.
	*/
	intrusive_ptr()
		: pointer(nullptr)
	{};

	/**
	* Initializes a new instance from the given @p pointer.
	* It is preferable to directly construct the type into an
	* intrusive_ptr with make_intrusive().
	* @param pointer The pointer that is to be held. May be null.
	*/
	explicit intrusive_ptr(T* pointer)
		: pointer(pointer)
	{
		if (pointer)
		{
			add_reference(pointer);
		}
	}

	/**
    * Initializes the @ref intrusive_ptr to a value corresponding to null.
	*/
	intrusive_ptr(std::nullptr_t) WENDA_NOEXCEPT
		: pointer(nullptr)
	{
	}

	/**
    * Copy constructor.
    * Copies the pointer, and if @p other is not equivalent to null,
    * adds a reference to the pointed-to object.
	*/
	intrusive_ptr(intrusive_ptr<T> const& other)
		: pointer(other.pointer)
	{
		if (pointer)
		{
			add_reference(pointer);
		}
	}

	/**
    * Move constructor. Moves the pointer, and nulls the source object.
	*/
	intrusive_ptr(intrusive_ptr<T>&& other) WENDA_NOEXCEPT
		: pointer(other.pointer)
	{
		other.pointer = nullptr;
	}

	intrusive_ptr& operator=(intrusive_ptr<T> const& other)
	{
		using std::swap;

		intrusive_ptr<T> temp(other);
		swap(*this, temp);
		return *this;
	}

	intrusive_ptr& operator=(intrusive_ptr<T>&& other) WENDA_NOEXCEPT
	{
		using std::swap;

		pointer = other.pointer;
		other.pointer = nullptr;

		return *this;
	}

	/**
    * Converts between @ref intrusive_ptr of compatible types. Two types @p T and @p U are compatible if
    * their respective pointer types are compatible.
    * @param other The pointer to be converted.
	*/
    template<typename U, typename SFINAE = std::enable_if<std::is_convertible<U*, T*>::value, void>::type>
	intrusive_ptr(intrusive_ptr<U> const& other)
		: pointer(other.pointer)
	{
		if (pointer)
		{
		    add_reference(pointer);
		}
	}

	/**
    * Converts between @ref intrusive_ptr of compatible types.
    * This overload moves the values instead of copying them.
	*/
    template<typename U, typename SFINAE = std::enable_if<std::is_convertible<U*, T*>::value, void>::type> 
	intrusive_ptr(intrusive_ptr<U>&& other) WENDA_NOEXCEPT
		: pointer(other.pointer)
	{
		other.pointer = nullptr;
	}

	/**
    * Assignment copy conversion operator.
	*/
    template<typename U, typename SFINAE = std::enable_if<std::is_convertible<U*, T*>::value, void>::type>
	intrusive_ptr<T>& operator=(intrusive_ptr<U> const& other)
	{
		using std::swap;

		intrusive_ptr<T> temp(other);

		swap(*this, temp);

		return *this;
	}

	/**
    * Assignment move conversion operator.
	*/
    template<typename U, typename SFINAE = std::enable_if<std::is_convertible<U*, T*>::value, void>::type> 
	intrusive_ptr<T>& operator=(intrusive_ptr<U>&& other) WENDA_NOEXCEPT
	{
		pointer = other.pointer;
		other.pointer = nullptr;

		return *this;
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

	/**
    * Gets the underlying naked pointer.
    * @returns The underlying pointer.
	*/
	T* get() WENDA_NOEXCEPT { return pointer; }
	/**
    * Gets the underlying naked pointer.
    * @returns The underlying pointer.
	*/
	T const* get() const WENDA_NOEXCEPT { return pointer; }

	/**
    * Dereferences this pointer.
    * @returns A reference to the pointed to object.
	*/
	T& operator*() WENDA_NOEXCEPT { return *pointer; }
	/**
    * Dereferences this pointer.
    * @returns A const reference to the pointed to object.
	*/
	T const& operator*() const WENDA_NOEXCEPT { return *pointer; }

	/**
    * Accesses the members of the pointed to object.
	*/
	T* operator->() WENDA_NOEXCEPT { return pointer; }
	/**
    * Accesses the const members of the pointed to object.
	*/
	T const* operator->() const WENDA_NOEXCEPT { return pointer; }

	/**
    * Explicit conversion to bool.
    * @returns True if the pointer is not null, otherwise true.
	*/
	explicit operator bool() const WENDA_NOEXCEPT
	{
		return pointer != nullptr;
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

namespace detail
{
    template<typename T1, typename U1>
	class const_cast_valid
	{
		template<typename T, typename U> static std::true_type test(typename std::add_pointer<decltype(const_cast<T*>(std::declval<U*>()))>::type);
		template<typename T, typename U> static std::false_type test(...);
	public:
		typedef decltype(test<T1, U1>(nullptr)) type;
		static const bool value = type::value;
	};
}

template<typename T, typename U>
intrusive_ptr<T> const_pointer_cast(intrusive_ptr<U> const& pointer) WENDA_NOEXCEPT
{
	static_assert(detail::const_cast_valid<T, U>::value, 
	    "const pointer cast is valid only if the similar const_cast between the raw pointer types would be valid.");
	return intrusive_ptr<T>(const_cast<T*>(pointer.get()));
}

template<typename T, typename U>
intrusive_ptr<T> const_pointer_cast(intrusive_ptr<U>&& pointer) WENDA_NOEXCEPT
{
	static_assert(detail::const_cast_valid<T, U>::value, 
	    "const pointer cast is valid only if the similar const_cast between the raw pointer types would be valid.");
	return intrusive_ptr<T>(std::move(pointer));
}

class intrusive_refcount
{
	friend std::size_t add_reference(intrusive_refcount const*);
    friend std::size_t remove_reference(intrusive_refcount const* obj);

	mutable std::atomic_size_t counter;
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

inline std::size_t add_reference(intrusive_refcount const* obj) WENDA_NOEXCEPT
{
    return ++(obj->counter);
}

inline std::size_t remove_reference(intrusive_refcount const* obj) WENDA_NOEXCEPT
{
	return --(obj->counter);
}

WENDA_FDS_NAMESPACE_END

#endif //WENDA_FDS_INTRUSIVE_PTR_H_INCLUDED