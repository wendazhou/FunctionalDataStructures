#ifndef WENDA_FDS_INTRUSIVE_PACKED_PTR_H_INCLUDED
#define WENDA_FDS_INTRUSIVE_PACKED_PTR_H_INCLUDED

#include "FDS_common.h"

#include "packed_ptr.h"
#include "intrusive_ptr.h"

WENDA_FDS_NAMESPACE_BEGIN

/**
* This class implements an @ref intrusive_ptr with a small integral value packed inside it.
* It is essentially a combination of @ref intrusive_ptr and @ref packed_ptr.
* The integral value is packed in the alignment bits of the raw pointer.
*/
template<typename T>
class intrusive_packed_ptr
	: public intrusive_ptr<T, packed_ptr<T>, packed_ptr<const T>, packed_ptr_deleter<T>>
{
public:
	/**
	* Default initializes this instance of @ref intrusive_packed_ptr.
	* This initializes the pointer to null and the packed integral value to 0.
	*/
	intrusive_packed_ptr()
		: intrusive_ptr()
	{}

	/**
	* Initializes the @ref intrusive_packed_ptr from a nullptr.
	* This initializes the pointer to null and the packed integral value to 0.
	*/
	intrusive_packed_ptr(std::nullptr_t)
		: intrusive_ptr(nullptr)
	{}

	/**
	* Initializes this instance from the given @p pointer
	* This instance is set to point to the given object, and has
	* an integral value of 0.
	*/
	intrusive_packed_ptr(T* pointer)
		: intrusive_ptr(pointer)
	{}

	/**
	* Copy constructor for @ref intrusive_packed_ptr.
	* Copies both the pointer and the packed integral value, and
	* increments the reference count on the pointed to object.
	*/
	intrusive_packed_ptr(intrusive_packed_ptr const& other)
		: intrusive_ptr(other)
	{}

	/**
	* Move constructor for @ref intrusive_packed_ptr.
	* Copies both the pointer and the packed integral value.
	*/
	intrusive_packed_ptr(intrusive_packed_ptr&& other)
		: intrusive_ptr(std::move(other))
	{}

	/**
	* Copy assign operator.
	*/
	intrusive_packed_ptr& operator=(intrusive_packed_ptr const& other)
	{
		intrusive_ptr::operator=(other);
		return *this;
	}

	/**
	* Move assign operator.
	*/
	intrusive_packed_ptr& operator=(intrusive_packed_ptr&& other)
	{
		intrusive_ptr::operator=(std::move(other));
		return *this;
	}

	/**
	* Gets the packed integral value.
	* @returns A @ref std::uint_fast32_t representing
	* the packed integral value.
	*/
	std::uint_fast32_t get_value() const
	{
		return pointer.get_value();
	}

	/**
	* Sets the packed integral to the given @p value.
	* The @p value must be less than the alignment of the pointed to type.
	* @param value The value to be set.
	*/
	void set_value(std::uint_fast32_t value)
	{
		pointer.set_value(value);
	}
};

WENDA_FDS_NAMESPACE_END

#endif // WENDA_FDS_INTRUSIVE_PACKED_PTR_H_INCLUDED