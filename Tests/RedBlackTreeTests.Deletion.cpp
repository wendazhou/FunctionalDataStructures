#include "stdafx.h"
#include <CppUnitTest.h>

#include <wenda/fds/redblack_tree.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace WENDA_FDS_NAMESPACE;
using namespace detail;

namespace tests
{
	TEST_CLASS(RedBlackTreeTests_Deletion)
	{
		TEST_METHOD(RemoveNode_Creates_Black_Leaf_Node_When_Removing_Red_Node_With_No_Children)
		{
			auto node = make_redblack_node<int>(0, NodeColour::Red, make_null_redblack_node<int>(), make_null_redblack_node<int>());
			auto result = remove_node<int>(node.get());

			Assert::IsTrue(NodeColour::Black == colour(result));
			Assert::IsTrue(is_leaf(result));
		}

		TEST_METHOD(RemoveNode_Creates_DoubleBlack_Leaf_Node_When_Removing_Black_Node_With_No_Children)
		{
			auto node = make_redblack_node<int>(0, NodeColour::Black, make_null_redblack_node<int>(), make_null_redblack_node<int>());
			auto result = remove_node<int>(node.get());

			Assert::IsTrue(NodeColour::DoubleBlack == colour(result));
			Assert::IsTrue(is_leaf(result));
		}

		TEST_METHOD(RemoveNode_Replaces_Child_Node_When_Removing_Node_With_One_Child)
		{
			auto right = make_redblack_node<int>(3, NodeColour::Red);
			auto node = make_redblack_node<int>(1, NodeColour::Black, make_null_redblack_node<int>(), right);

			auto result = remove_node<int>(node.get());

			Assert::IsTrue(NodeColour::Black == colour(result));
			Assert::IsTrue(is_leaf(result->get_left()));
			Assert::IsTrue(is_leaf(result->get_right()));

			Assert::AreEqual(3, result->get_data());
		}
	};
}