#include "stdafx.h"
#include <CppUnitTest.h>

#include <wenda/fds/redblack_tree.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace WENDA_FDS_NAMESPACE;

namespace tests
{
	TEST_CLASS(RedBlackTreeTests)
	{
		TEST_METHOD(RedBlackTree_Default_IsEmpty)
		{
			const redblack_tree<int> tree;

			Assert::IsTrue(tree.empty());
		}

		TEST_METHOD(RedBlackTree_Default_Begin_Equals_End)
		{
			const redblack_tree<int> tree;
			Assert::IsTrue(tree.begin() == tree.end());
		}

		TEST_METHOD(RedBlackTree_Default_CBegin_Equals_CEnd)
		{
			const redblack_tree<int> tree;
			Assert::IsTrue(tree.cbegin() == tree.cend());
		}

		TEST_METHOD(RedBlackTree_Default_Find_Returns_End)
		{
			const redblack_tree<int> tree;
			Assert::IsTrue(tree.end() == tree.find(5));
		}

		TEST_METHOD(RedBlackTree_Can_Insert_Element)
		{
			using std::get;

			const redblack_tree<int> tree;
			auto newTree = get<0>(tree.insert(5));

			auto found = newTree.find(5);

			Assert::IsTrue(found != newTree.end());
			Assert::AreEqual(5, *found);
		}

		TEST_METHOD(RedBlackTree_Can_Assign)
		{
			using std::get;

			redblack_tree<int> tree;
			tree = get<0>(tree.insert(5));

			auto found = tree.find(5);

			Assert::IsTrue(found != tree.end());
			Assert::AreEqual(5, *found);
		}

		TEST_METHOD(RedBlackTree_Can_Assign_With_Tie)
		{
			redblack_tree<int> tree;
			std::tie(tree, std::ignore, std::ignore) = tree.insert(5);

			auto found = tree.find(5);

			Assert::IsTrue(found != tree.end());
			Assert::AreEqual(5, *found);
		}

		void TestInsert(int count)
		{
			redblack_tree<int> tree;

			for (int i = 0; i < count; i++)
			{
				std::tie(tree, std::ignore, std::ignore) = tree.insert(i);
			}

			for (int i = 0; i < count; i++)
			{
				auto found = tree.find(i);

				Assert::IsTrue(found != tree.end());
				Assert::AreEqual(i, *found);
			}
		}

		TEST_METHOD(RedBlackTree_Can_Insert_Multiple_Elements_2)
		{
			TestInsert(2);
		}

		TEST_METHOD(RedBlackTree_Can_Insert_Multiple_Elements_3)
		{
			TestInsert(3);
		}

		TEST_METHOD(RedBlackTree_Can_Insert_Multiple_Elements_10)
		{
			TestInsert(10);
		}
	};
}