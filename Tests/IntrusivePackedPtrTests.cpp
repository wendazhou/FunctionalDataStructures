#include "stdafx.h"
#include <CppUnitTest.h>

#include <wenda/fds/intrusive_packed_ptr.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace WENDA_FDS_NAMESPACE;

namespace tests
{
	namespace
	{
		struct ref_counted_int
			: intrusive_refcount
		{
			int value;
		};
	}

	TEST_CLASS(IntrusivePackedPtrTests)
	{
		TEST_METHOD(IntrusivePackedPtr_Default_Initializes_To_Null)
		{
			intrusive_packed_ptr<intrusive_refcount> ptr;
			Assert::IsFalse(static_cast<bool>(ptr));
			Assert::IsTrue(ptr == nullptr);
			Assert::IsTrue(nullptr == ptr);
		}

		TEST_METHOD(IntrusivePackedPtr_Can_Access_Using_Arrow_Overload)
		{
			intrusive_packed_ptr<ref_counted_int> ptr = new ref_counted_int();

			ptr->value = 5;

			Assert::AreEqual(5, ptr->value);
		}

		TEST_METHOD(IntrusivePackedPtr_Can_Access_Using_Dereferenc_Overload)
		{
			intrusive_packed_ptr<ref_counted_int> ptr = new ref_counted_int();

			(*ptr).value = 5;

			Assert::AreEqual(5, (*ptr).value);
		}

		TEST_METHOD(IntrusivePackedPtr_Can_Get_Set_Value)
		{
			intrusive_packed_ptr<ref_counted_int> ptr = new ref_counted_int();

			ptr.set_value(2);

			Assert::AreEqual(2u, ptr.get_value());
		}

		TEST_METHOD(IntrusivePackedPtr_Copy_Preserves_Value)
		{
			intrusive_packed_ptr<ref_counted_int> ptr = new ref_counted_int();
			ptr.set_value(2);

			intrusive_packed_ptr<ref_counted_int> ptr2(ptr);

			Assert::AreEqual(2u, ptr2.get_value());
		}

		TEST_METHOD(IntrusivePackedPtr_Move_Preserves_Value)
		{
			intrusive_packed_ptr<ref_counted_int> ptr = new ref_counted_int();
			ptr.set_value(2);

			intrusive_packed_ptr<ref_counted_int> ptr2(std::move(ptr));

			Assert::AreEqual(2u, ptr2.get_value());
		}

		TEST_METHOD(IntrusivePackedPtr_Copy_Assign_Preserves_Value)
		{
			intrusive_packed_ptr<ref_counted_int> ptr = new ref_counted_int();
			ptr.set_value(2);

			intrusive_packed_ptr<ref_counted_int> ptr2;

			ptr2 = ptr;

			Assert::AreEqual(2u, ptr2.get_value());
		}

		TEST_METHOD(IntrusivePackedPtr_Move_Assign_Preserves_Value)
		{
			intrusive_packed_ptr<ref_counted_int> ptr = new ref_counted_int();
			ptr.set_value(2);

			intrusive_packed_ptr<ref_counted_int> ptr2;

			ptr2 = std::move(ptr);

			Assert::AreEqual(2u, ptr2.get_value());
		}
	};
}