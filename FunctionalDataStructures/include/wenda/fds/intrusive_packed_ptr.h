#ifndef WENDA_FDS_INTRUSIVE_PACKED_PTR_H_INCLUDED
#define WENDA_FDS_INTRUSIVE_PACKED_PTR_H_INCLUDED

#include "FDS_common.h"

#include "packed_ptr.h"
#include "intrusive_ptr.h"

WENDA_FDS_NAMESPACE_BEGIN

template<typename T>
class intrusive_packed_ptr
	: public intrusive_ptr<T, packed_ptr<T>, packed_ptr<const T>, packed_ptr_deleter<T>>
{
public:
	intrusive_packed_ptr()
		: intrusive_ptr()
	{}

	intrusive_packed_ptr(std::nullptr_t)
		: intrusive_ptr(nullptr)
	{}

	intrusive_packed_ptr(T* pointer)
		: intrusive_ptr(pointer)
	{}

	intrusive_packed_ptr(intrusive_packed_ptr const& other)
		: intrusive_ptr(other)
	{}

	intrusive_packed_ptr(intrusive_packed_ptr&& other)
		: intrusive_ptr(std::move(other))
	{}

	intrusive_packed_ptr& operator=(intrusive_packed_ptr const& other)
	{
		intrusive_ptr::operator=(other);
		return *this;
	}

	intrusive_packed_ptr& operator=(intrusive_packed_ptr&& other)
	{
		intrusive_ptr::operator=(std::move(other));
		return *this;
	}

	std::uint_fast32_t get_value() const
	{
		return pointer.get_value();
	}

	void set_value(std::uint_fast32_t value)
	{
		pointer.set_value(value);
	}
};

WENDA_FDS_NAMESPACE_END

#endif // WENDA_FDS_INTRUSIVE_PACKED_PTR_H_INCLUDED