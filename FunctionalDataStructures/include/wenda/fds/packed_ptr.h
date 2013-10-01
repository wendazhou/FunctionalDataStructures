#ifndef WENDA_FDS_PACKED_PTR_H_INCLUDED
#define WENDA_FDS_PACKED_PTR_H_INCLUDED

#include "FDS_common.h"
#include <cstdint>
#include <type_traits>
#include <cassert>

/**
* @file packed_ptr.h
* This file implements a pointer that is able to pack an integral value in
* its alignment.
*/

WENDA_FDS_NAMESPACE_BEGIN

/**
* This class implements a pointer type with a small integral type packed in.
* It uses the least significant bit of the pointers, which are always zero
* due to the aligment of the pointed to type to pack an integral value.
* @tparam T The type pointed to by this pointer.
*/
template<typename T>
class packed_ptr
{
	template<typename U> friend class packed_ptr;
	template<typename T> friend bool operator==(packed_ptr<T> const&, packed_ptr<T> const&);
	template<typename T> friend bool operator!=(packed_ptr<T> const&, packed_ptr<T> const&);

	std::uintptr_t ptr;

	std::uintptr_t get_pointer_value() const WENDA_NOEXCEPT
	{
		return ptr & ~(std::alignment_of<T>::value - 1);
	}

	T* get_pointer() WENDA_NOEXCEPT
	{
		return reinterpret_cast<T*>(get_pointer_value());
	}

	T const* get_pointer() const WENDA_NOEXCEPT
	{
		return reinterpret_cast<T const*>(get_pointer_value());
	}

	std::uintptr_t get_packed_value() const WENDA_NOEXCEPT
	{
		return ptr & (std::alignment_of<T>::value - 1);
	}

	void set_packed_value(std::uintptr_t value) WENDA_NOEXCEPT
	{
		assert(value < std::alignment_of<T>::value);
		ptr = get_pointer_value() | value;
	}
public:
	/**
	* Default initializes a pointer to the required type.
	*/
	packed_ptr() = default;

	/*
	* Initializes the pointer to a null value.
	* This sets the pointed to object to null,
	* and the packed integral type to 0.
	*/
	WENDA_CONSTEXPR packed_ptr(std::nullptr_t) WENDA_NOEXCEPT
		: ptr(0)
	{}

	/**
	* Initializes the pointer to point to a given object.
	* The integral value is set to 0.
	*/
	WENDA_CONSTEXPR packed_ptr(T* ptr) WENDA_NOEXCEPT
		: ptr(reinterpret_cast<std::uintptr_t>(ptr))
	{}

	/**
	* Conversion operator for compatible pointer types.
	* If the two pointer types are convertible, then the
	* corresponding @ref packed_ptr are also convertible.
	*/
	template<typename U, typename SFINAE = std::enable_if<std::is_convertible<U*, T*>::value>::type>
	packed_ptr(packed_ptr<U> const& other) WENDA_NOEXCEPT
		: ptr(other.ptr)
	{
	}

	/**
	* Gets the underlying raw pointer.
	* @returns The underlying raw pointer.
	*/
	T* get()
	{
		return get_pointer();
	}

	/**
	* Gets the underlying raw pointer for a const object.
	* @returns The underlying raw pointer.
	*/
	T const* get() const
	{
		return get_pointer();
	}

	/**
	* Dereferences this pointer to access the pointed to object.
	* Note that the pointer must not be null in order for this operation to be valid.
	* @returns A reference to the pointed to object.
	*/
	T& operator*() WENDA_NOEXCEPT
	{
		return *get_pointer();
	}

	/**
	* Derefences a const pointer to access the pointed to object.
	* Note that the pointer must not be null in order for this operation to be valid.
	* @returns A const reference to the pointed to object.
	*/
	T const& operator*() const WENDA_NOEXCEPT
	{
		return *get_pointer();
	}

	/**
	* Indirect access to the pointed to object.
	* Note that the pointer must not be null in order for this operation to be valid.
	*/
	T* operator->() WENDA_NOEXCEPT
	{
		return get_pointer();
	}

	/**
	* Const indirect access to the pointed to object.
	* Note that the pointer must not be null in order for this operation to be valid.
	*/
	T const* operator->() const WENDA_NOEXCEPT
	{
		return get_pointer();
	}


	/**
	* Explicit operator bool.
	* Determines whether the pointer points to an object or null.
	* Note that the integral value set does not affect the return value of this operation.
	* @returns True if the underlying pointer is non-null; otherwise false.
	*/
	explicit operator bool() const WENDA_NOEXCEPT
	{
		return get_pointer() != nullptr;
	}

	/**
	* Gets the packed value.
	* @returns The packed integral value currently set.
	*/
	std::uint_fast32_t get_value() const WENDA_NOEXCEPT
	{
		return static_cast<std::uint32_t>(get_packed_value());
	}

	/**
	* Sets the packed integral value.
	* Note that the @p value must be strictly less than the alignment of the pointed to type.
	* @param value The value to set.
	*/
	void set_value(std::uint_fast32_t value) WENDA_NOEXCEPT
	{
		set_packed_value(value);
	}
};

template<typename T>
bool operator==(packed_ptr<T> const& left, packed_ptr<T> const& right) WENDA_NOEXCEPT
{
	return left.ptr == right.ptr;
}

template<typename T>
bool operator!=(packed_ptr<T> const& right, packed_ptr<T> const& left) WENDA_NOEXCEPT
{
	return left.ptr != right.ptr;
}

template<typename T>
bool operator==(std::nullptr_t, packed_ptr<T> const& right) WENDA_NOEXCEPT
{
	return packed_ptr<T>(nullptr) == right;
}

template<typename T>
bool operator==(packed_ptr<T> const& left, std::nullptr_t) WENDA_NOEXCEPT
{
	return packed_ptr<T>(nullptr) == left;
}

template<typename T>
bool operator!=(packed_ptr<T> const& left, std::nullptr_t) WENDA_NOEXCEPT
{
	return packed_ptr<T>(nullptr) != left;
}

template<typename T>
bool operator!=(std::nullptr_t, packed_ptr<T> const& right) WENDA_NOEXCEPT
{
	return packed_ptr<T>(nullptr) != right;
}

/**
* Deletes the object pointed to by the passed in @p ptr.
* @param ptr A pointer to the object to be deleted.
*/
template<typename T>
void delete_ptr(packed_ptr<T> const& ptr) WENDA_NOEXCEPT
{
	delete ptr.get();
}

/**
* Increments the reference count for the given @ref packed_ptr.
* In order for this to succeed, there must exist an overload of
* add_reference() for the raw pointer type.
* @param ptr A pointer pointing to the object for which to increment the refence count.
*/
template<typename T>
std::size_t add_reference(packed_ptr<T> const& ptr) WENDA_NOEXCEPT
{
	return add_reference(ptr.get());
}

/**
* Decrements the reference count for the given @ref packed_ptr.
* In order for this co succeed, there must exist an overload of
* remove_refence() for the raw pointer type.
*/
template<typename T>
std::size_t remove_reference(packed_ptr<T> const& ptr) WENDA_NOEXCEPT
{
	return remove_reference(ptr.get());
}

/**
* Custom deleter for the @ref packed_ptr type.
* This struct models a unary function that
* deletes on the passed in @ref packed_ptr.
* This class essentially wraps a functor around delete_ptr()
* @sa delete_ptr()
*/
template<typename T>
struct packed_ptr_deleter
{
	/**
	* Deletes the object referenced by the @p pointer.
	* @param pointer A pointer pointing to the object to be deleted.
	*/
	void operator()(packed_ptr<T> const& pointer) const
	{
		delete_ptr(pointer);
	}

	/**
	* Deletes the object referenced by the @p pointer.
	* @param pointer A pointer pointing to the object to be deleted.
	*/
	void operator()(packed_ptr<const T> const& pointer) const
	{
		delete_ptr(pointer);
	}
};

WENDA_FDS_NAMESPACE_END

#endif // WENDA_FDS_PACKED_PTR_H_INCLUDED