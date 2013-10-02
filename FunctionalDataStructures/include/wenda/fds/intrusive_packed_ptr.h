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
	intrusive_packed_ptr() WENDA_NOEXCEPT
		: intrusive_ptr()
	{}

	/**
	* Initializes the @ref intrusive_packed_ptr from a nullptr.
	* This initializes the pointer to null and the packed integral value to 0.
	*/
	intrusive_packed_ptr(std::nullptr_t) WENDA_NOEXCEPT
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
	* Initializes this instance from the given @ref packed_ptr.
	* @param pointer The @ref packed_pointer from which to initialize @ref intrusive_packed_ptr.
	*/
	intrusive_packed_ptr(packed_ptr<T> pointer)
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
	intrusive_packed_ptr(intrusive_packed_ptr&& other) WENDA_NOEXCEPT
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
	intrusive_packed_ptr& operator=(intrusive_packed_ptr&& other) WENDA_NOEXCEPT
	{
		intrusive_ptr::operator=(std::move(other));
		return *this;
	}

	/**
	* Copy convert operator.
	* Converts from a compatible pointer type.
	*/
	template<typename U, typename SFINAE = typename std::enable_if<std::is_convertible<U*, T*>::value>::type>
	intrusive_packed_ptr(intrusive_packed_ptr<U> const& other)
		: intrusive_ptr(other.get())
	{}

	/**
	* Move converts operator.
	* Converts from a compatible pointer type.
	*/
	template<typename U, typename SFINAE = typename std::enable_if<std::is_convertible<U*, T*>::value>::type>
	intrusive_packed_ptr(intrusive_packed_ptr<U>&& other)
		: intrusive_ptr(std::move(other.get()))
	{}

	/**
	* Gets the packed integral value.
	* @returns A @ref std::uint_fast32_t representing
	* the packed integral value.
	*/
	std::uint_fast32_t get_value() const WENDA_NOEXCEPT
	{
		return pointer.get_value();
	}

	/**
	* Sets the packed integral to the given @p value.
	* The @p value must be less than the alignment of the pointed to type.
	* @param value The value to be set.
	*/
	void set_value(std::uint_fast32_t value) WENDA_NOEXCEPT
	{
		pointer.set_value(value);
	}
};

template<typename T, typename... Args>
intrusive_packed_ptr<T> make_intrusive_packed(Args... args)
{
	auto ptr = new T(std::forward<Args>(args)...);
	return intrusive_packed_ptr<T>(ptr);
}

WENDA_FDS_NAMESPACE_END

#endif // WENDA_FDS_INTRUSIVE_PACKED_PTR_H_INCLUDED