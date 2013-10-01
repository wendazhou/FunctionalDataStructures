#include "stdafx.h"
#include <CppUnitTest.h>

#include <wenda/fds/packed_ptr.h>
#include <wenda/fds/intrusive_ptr.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace WENDA_FDS_NAMESPACE;

namespace tests
{
	TEST_CLASS(PackedPtrTests)
	{
		TEST_METHOD(PackedPtr_Can_Initialize_From_Null)
		{
			packed_ptr<int> ptr(nullptr);
			Assert::IsTrue(!ptr);
		}

		TEST_METHOD(PackedPtr_Can_Initialize_From_Ptr)
		{
			packed_ptr<int> ptr = new int(5);

			Assert::IsTrue(ptr != nullptr);
			Assert::AreEqual(5, *ptr);

			delete_ptr(ptr);
		}

		TEST_METHOD(PackedPtr_Can_Compare)
		{
			packed_ptr<int> ptr = new int(5);
			auto ptr2 = ptr;

			Assert::IsTrue(ptr == ptr2);

			delete_ptr(ptr);
		}

		TEST_METHOD(PackedPtr_Can_Store_Value)
		{
			packed_ptr<int> ptr = new int(5);
			ptr.set_value(2);

			Assert::AreEqual(2u, ptr.get_value());
			Assert::AreEqual(5, *ptr);

			delete_ptr(ptr);
		}

		TEST_METHOD(PackedPtr_Can_Modify_Object_Through_Ptr)
		{
			packed_ptr<int> ptr = new int(5);
			*ptr = 10;

			Assert::AreEqual(10, *ptr);

			delete_ptr(ptr);
		}

		TEST_METHOD(PackedPtr_Value_Set_Does_Not_Compare_Equal)
		{
			packed_ptr<int> ptr = new int(5);
			auto ptr2 = ptr;
			ptr2.set_value(1);

			Assert::IsTrue(ptr2 != ptr);
			Assert::IsFalse(ptr2 == ptr);
		}

		TEST_METHOD(PackedPtr_Can_Set_Value_On_Nullptr)
		{
			packed_ptr<int> ptr{ nullptr };
			ptr.set_value(2);

			Assert::AreEqual(2u, ptr.get_value());
		}

		TEST_METHOD(PackedPtr_Nullptr_With_Value_NotEqual_Nullptr)
		{
			packed_ptr<int> ptr{ nullptr };
			ptr.set_value(2);

			Assert::IsTrue(ptr != nullptr);
			Assert::IsFalse(ptr == nullptr);
		}

		TEST_METHOD(PackedPtr_Nullptr_Evaluates_To_False)
		{
			packed_ptr<int> ptr{ nullptr };
			
			Assert::IsFalse(static_cast<bool>(ptr));
		}

		TEST_METHOD(PackedPtr_Nullptr_With_Value_Evaluates_To_False)
		{
			packed_ptr<int> ptr{ nullptr };
			ptr.set_value(2);

			Assert::IsFalse(static_cast<bool>(ptr));
		}

		TEST_METHOD(PackedPtr_To_IntrusiveRefCount_Can_Add_Reference)
		{
			packed_ptr<intrusive_refcount> ptr = new intrusive_refcount();

			int refCount = add_reference(ptr);
			Assert::AreEqual(1, refCount);
			refCount = remove_reference(ptr);
			Assert::AreEqual(0, refCount);
			
			delete_ptr(ptr);
		}
	};
}