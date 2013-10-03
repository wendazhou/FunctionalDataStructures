#include "stdafx.h"
#include <CppUnitTest.h>

#include <wenda/fds/redblack_tree.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace WENDA_FDS_NAMESPACE;
using namespace detail;

namespace tests
{
	TEST_CLASS(RedBlackTreeTests_Balance)
	{
		TEST_METHOD(Balance_Work_Correctly_For_Double_Red_Left_Left_With_Black_GrandParent)
		{
			auto leftLeft = make_redblack_node<int>(1);
			auto left = make_redblack_node<int>(2, NodeColour::Red, leftLeft);
			auto right = make_null_redblack_node<int>();

			const_rb_pointer<int> inserted = nullptr;

			auto result = balance<int>(NodeColour::Black, 3, left, right, inserted);

			Assert::IsTrue(NodeColour::Red == colour(result));
			Assert::IsTrue(!is_leaf(result->get_left()));
			Assert::IsTrue(!is_leaf(result->get_right()));

			Assert::AreEqual(1, result->get_left()->get_data());
			Assert::AreEqual(2, result->get_data());
			Assert::AreEqual(3, result->get_right()->get_data());
		}

		TEST_METHOD(Balance_Work_Correctly_For_Double_Red_Left_Right_With_DoubleBlack_GrandParent)
		{
			auto leftRight = make_redblack_node<int>(2);
			auto left = make_redblack_node<int>(1, NodeColour::Red, make_null_redblack_node<int>(), leftRight);
			auto right = make_null_redblack_node<int>();

			const_rb_pointer<int> inserted = nullptr;

			auto result = balance<int>(NodeColour::DoubleBlack, 3, left, right, inserted);

			Assert::IsTrue(NodeColour::Black == colour(result), L"Incorrect colour.");
			Assert::IsTrue(!is_leaf(result->get_left()), L"left node is leaf when it should not be.");
			Assert::IsTrue(!is_leaf(result->get_right()), L"right node is leaf when it should note be.");

			Assert::AreEqual(1, result->get_left()->get_data());
			Assert::AreEqual(2, result->get_data());
			Assert::AreEqual(3, result->get_right()->get_data());
		}

		TEST_METHOD(Balance_Work_Correctly_For_Double_Red_Right_Right_With_Black_GrandParent)
		{
			auto rightRight = make_redblack_node<int>(4);
			auto right = make_redblack_node<int>(3, NodeColour::Red, make_null_redblack_node<int>(), rightRight);
			auto left = make_null_redblack_node<int>();

			const_rb_pointer<int> inserted = nullptr;

			auto result = balance<int>(NodeColour::Black, 2, left, right, inserted);

			Assert::IsTrue(NodeColour::Red == colour(result), L"Incorrect colour.");
			Assert::IsTrue(!is_leaf(result->get_left()), L"left node is leaf when it should not be.");
			Assert::IsTrue(!is_leaf(result->get_right()), L"right node is leaf when it should note be.");

			Assert::AreEqual(2, result->get_left()->get_data());
			Assert::AreEqual(3, result->get_data());
			Assert::AreEqual(4, result->get_right()->get_data());
		}

		TEST_METHOD(Balance_Work_Correctly_For_Double_Red_Right_Left_With_Black_GrandParent)
		{
			auto rightLeft = make_redblack_node<int>(3);
			auto right = make_redblack_node<int>(4, NodeColour::Red, rightLeft);
			auto left = make_null_redblack_node<int>();

			const_rb_pointer<int> inserted = nullptr;

			auto result = balance<int>(NodeColour::Black, 2, left, right, inserted);

			Assert::IsTrue(NodeColour::Red == colour(result), L"Incorrect colour.");
			Assert::IsTrue(!is_leaf(result->get_left()), L"left node is leaf when it should not be.");
			Assert::IsTrue(!is_leaf(result->get_right()), L"right node is leaf when it should note be.");

			Assert::AreEqual(2, result->get_left()->get_data());
			Assert::AreEqual(3, result->get_data());
			Assert::AreEqual(4, result->get_right()->get_data());
		}

		TEST_METHOD(BubbleBalance_Works_Correctly_For_NegativeBlack_Tree)
		{
			auto leftLeft = make_redblack_node<int>(1, NodeColour::Black);
			auto leftRight = make_redblack_node<int>(3, NodeColour::Black);
			auto left = make_redblack_node<int>(2, NodeColour::NegativeBlack, leftLeft, leftRight);

			auto result = bubble_balance<int>(NodeColour::DoubleBlack, 4, left, make_null_redblack_node<int>());

			Assert::IsTrue(NodeColour::Black == colour(result));
			Assert::AreEqual(3, result->get_data());

			Assert::IsTrue(NodeColour::Black == colour(result->get_right()));
			Assert::AreEqual(4, result->get_right()->get_data());

			Assert::AreEqual(2, result->get_left()->get_data());
		}
	};
}