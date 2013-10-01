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
	template<typename T> friend bool operator==(packed_ptr<T> const&, packed_ptr<T> const&);
	template<typename T> friend bool operator!=(packed_ptr<T> const&, packed_ptr<T> const&);

	std::uintptr_t ptr;

	std::uintptr_t get_pointer_value() const
	{
		return ptr & ~(std::alignment_of<T>::value - 1);
	}

	T* get_pointer()
	{
		return reinterpret_cast<T*>(get_pointer_value());
	}

	T const* get_pointer() const
	{
		return reinterpret_cast<T const*>(get_pointer_value());
	}

	std::uintptr_t get_packed_value() const
	{
		return ptr & (std::alignment_of<T>::value - 1);
	}

	void set_packed_value(std::uintptr_t value)
	{
		assert(value < std::alignment_of<T>::value);
		ptr = get_pointer_value() | value;
	}
public:
	packed_ptr() = default;

	packed_ptr(std::nullptr_t)
		: ptr(0)
	{}

	packed_ptr(T* ptr)
		: ptr(reinterpret_cast<std::uintptr_t>(ptr))
	{}

	operator T*()
	{
		return get_pointer();
	}

	operator T const*() const
	{
		return get_pointer();
	}

	T& operator*()
	{
		return *get_pointer();
	}

	T const& operator*() const
	{
		return *get_pointer();
	}

	T* operator->()
	{
		return get_pointer();
	}

	T const* operator->() const
	{
		return get_pointer();
	}

	std::uint_fast32_t get_value() const
	{
		return static_cast<std::uint32_t>(get_packed_value());
	}

	void set_value(std::uint_fast32_t value)
	{
		set_packed_value(value);
	}
};

template<typename T>
bool operator==(packed_ptr<T> const& left, packed_ptr<T> const& right)
{
	return left.ptr == right.ptr;
}

template<typename T>
bool operator!=(packed_ptr<T> const& right, packed_ptr<T> const& left)
{
	return left.ptr != right.ptr;
}

template<typename T>
bool operator==(std::nullptr_t, packed_ptr<T> const& right)
{
	return packed_ptr<T>(nullptr) == right;
}

template<typename T>
bool operator==(packed_ptr<T> const& left, std::nullptr_t)
{
	return packed_ptr<T>(nullptr) == left;
}

WENDA_FDS_NAMESPACE_END

#endif // WENDA_FDS_PACKED_PTR_H_INCLUDED