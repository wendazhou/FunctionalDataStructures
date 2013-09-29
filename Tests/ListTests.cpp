#include "stdafx.h"
#include <CppUnitTest.h>

#include <string>

#include <wenda/fds/List.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace WENDA_FDS_NAMESPACE;

namespace tests
{
	TEST_CLASS(ListTests)
	{
		TEST_METHOD(List_Default_Is_Empty)
		{
			const List<int> list;
			Assert::IsTrue(list.empty());
		}

		TEST_METHOD(List_Can_PushFront_RValue)
		{
			const List<int> list;
			auto newList = list.push_front(5);

			Assert::AreEqual(5, newList.front());
		}

		TEST_METHOD(List_Can_PushFront_LValue)
		{
			const List<int> list;
			auto value = 6;
			auto newList = list.push_front(value);

			Assert::AreEqual(value, newList.front());
		}

		TEST_METHOD(List_Can_EmplaceFront)
		{
			const List<std::string> list;
			auto value = "abcd";
			auto newList = list.emplace_front(value);

			Assert::AreEqual(std::string(value), newList.front());
		}
	};
}