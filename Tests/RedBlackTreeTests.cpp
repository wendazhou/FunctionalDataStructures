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
			redblack_tree<int> tree;

			Assert::IsTrue(tree.empty());
		}

		TEST_METHOD(RedBlackTree_Default_Begin_Equals_End)
		{
			redblack_tree<int> tree;
			Assert::IsTrue(tree.begin() == tree.end());
		}

		TEST_METHOD(RedBlackTree_Default_CBegin_Equals_CEnd)
		{
			redblack_tree<int> tree;
			Assert::IsTrue(tree.cbegin() == tree.cend());
		}
	};
}