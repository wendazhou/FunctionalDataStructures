#include "stdafx.h"
#include <CppUnitTest.h>

#include <string>

#include <wenda/fds/forward_list.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace WENDA_FDS_NAMESPACE;

namespace tests
{
	TEST_CLASS(ForwardListTests)
	{
		TEST_METHOD(ForwardList_Default_Is_Empty)
		{
			const forward_list<int> forward_list;
			Assert::IsTrue(forward_list.empty());
		}

		TEST_METHOD(ForwardList_Can_PushFront_RValue)
		{
			const forward_list<int> forward_list;
			auto newforward_list = forward_list.push_front(5);

			Assert::AreEqual(5, newforward_list.front());
		}

		TEST_METHOD(ForwardList_Can_PushFront_LValue)
		{
			const forward_list<int> forward_list;
			auto value = 6;
			auto newforward_list = forward_list.push_front(value);

			Assert::AreEqual(value, newforward_list.front());
		}

		TEST_METHOD(ForwardList_Can_EmplaceFront)
		{
			const forward_list<std::string> forward_list;
			auto value = "abcd";
			auto newforward_list = forward_list.emplace_front(value);

			Assert::AreEqual(std::string(value), newforward_list.front());
		}
	};
}