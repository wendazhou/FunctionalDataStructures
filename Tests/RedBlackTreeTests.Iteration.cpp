#include "stdafx.h"
#include <CppUnitTest.h>

#include <wenda/fds/redblack_tree.h>

#include <set>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace WENDA_FDS_NAMESPACE;

namespace tests
{
	TEST_CLASS(RedBlackTreeTests_Iteration)
	{
		TEST_METHOD(RedBlackTree_Can_Iterate_Prefix_Singleton)
		{
			redblack_tree<int> tree;
			std::tie(tree, std::ignore, std::ignore) = tree.insert(1);

			auto beg = tree.begin();

			Assert::AreEqual(1, *beg);
			++beg;

			Assert::IsTrue(beg == tree.end());
		}

		TEST_METHOD(RedBlackTree_Can_Iterate_Postfix_Singleton)
		{
			redblack_tree<int> tree;
			std::tie(tree, std::ignore, std::ignore) = tree.insert(1);

			auto beg = tree.begin();

			auto beg2 = beg++;

			Assert::AreEqual(1, *beg2);
			++beg;

			Assert::IsTrue(beg == tree.end());
		}

		TEST_METHOD(RedBlackTree_Can_Iterate_10_Elements)
		{
			redblack_tree<int> tree;
			std::set<int> set;

			for (int i = 0; i < 10; i++)
			{
				std::tie(tree, std::ignore, std::ignore) = tree.insert(i);
				set.insert(i);
			}

			Assert::IsTrue(std::equal(tree.begin(), tree.end(), set.begin()));
		}
	};
}