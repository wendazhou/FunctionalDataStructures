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

		TEST_METHOD(RedBlackTree_Can_Insert_Delete_Node)
		{
			using std::get;

			redblack_tree<int> tree;
			std::tie(tree, std::ignore, std::ignore) = tree.insert(1);

			auto result = tree.erase(1);

			Assert::IsTrue(get<1>(result));
			Assert::IsTrue(get<0>(result).empty());
		}

		TEST_METHOD(RedBlackTree_Can_Insert_Delete_Two_Nodes_Parent)
		{
			using std::get;

			redblack_tree<int> tree;
			std::tie(tree, std::ignore, std::ignore) = tree.insert(1);
			std::tie(tree, std::ignore, std::ignore) = tree.insert(2);

			bool deleted;

			std::tie(tree, deleted) = tree.erase(1);

			Assert::IsTrue(deleted, L"node note reported as deleted.");
			Assert::IsTrue(tree.find(2) != tree.end(), L"could not find not deleted node.");
			Assert::IsTrue(tree.find(1) == tree.end(), L"deleted node still exists.");
		}

		TEST_METHOD(RedBlackTree_Can_Insert_Delete_Parent_With_Two_Children)
		{
			using std::get;

			redblack_tree<int> tree;
			std::tie(tree, std::ignore, std::ignore) = tree.insert(1);
			std::tie(tree, std::ignore, std::ignore) = tree.insert(2);
			std::tie(tree, std::ignore, std::ignore) = tree.insert(3);

			bool deleted;
			std::tie(tree, deleted) = tree.erase(2);

			Assert::IsTrue(deleted);
			Assert::IsTrue(tree.find(1) != tree.end(), L"could not find not deleted node.");
			Assert::IsTrue(tree.find(3) != tree.end(), L"could not find not deleted node.");
			Assert::IsTrue(tree.find(2) == tree.end(), L"deleted node still exists.");
		}
	};
}