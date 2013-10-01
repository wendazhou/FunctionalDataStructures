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
	packed_ptr() = default;

	WENDA_CONSTEXPR packed_ptr(std::nullptr_t) WENDA_NOEXCEPT
		: ptr(0)
	{}

	WENDA_CONSTEXPR packed_ptr(T* ptr) WENDA_NOEXCEPT
		: ptr(reinterpret_cast<std::uintptr_t>(ptr))
	{}

	template<typename U, typename SFINAE = std::enable_if<std::is_convertible<U*, T*>::value>::type>
	packed_ptr(packed_ptr<U> const& other) WENDA_NOEXCEPT
		: ptr(other.ptr)
	{
	}

	explicit operator T*() WENDA_NOEXCEPT
	{
		return get_pointer();
	}

	explicit operator T const*() const WENDA_NOEXCEPT
	{
		return get_pointer();
	}

	T& operator*() WENDA_NOEXCEPT
	{
		return *get_pointer();
	}

	T const& operator*() const WENDA_NOEXCEPT
	{
		return *get_pointer();
	}

	T* operator->() WENDA_NOEXCEPT
	{
		return get_pointer();
	}

	T const* operator->() const WENDA_NOEXCEPT
	{
		return get_pointer();
	}

	explicit operator bool() const WENDA_NOEXCEPT
	{
		return get_pointer() != nullptr;
	}

	std::uint_fast32_t get_value() const WENDA_NOEXCEPT
	{
		return static_cast<std::uint32_t>(get_packed_value());
	}

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

template<typename T>
void delete_ptr(packed_ptr<T>& ptr) WENDA_NOEXCEPT
{
	delete static_cast<T const*>(ptr);
}

template<typename T>
std::size_t add_reference(packed_ptr<T> const& ptr) WENDA_NOEXCEPT
{
	return add_reference(static_cast<T const*>(ptr));
}

template<typename T>
std::size_t remove_refence(packed_ptr<T> const& ptr) WENDA_NOEXCEPT
{
	return remove_refence(static_cast<T const*>(ptr));
}

WENDA_FDS_NAMESPACE_END

#endif // WENDA_FDS_PACKED_PTR_H_INCLUDED