#include "stdafx.h"
#include <CppUnitTest.h>

#include <wenda/fds/redblack_tree.h>

#include <vector>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace WENDA_FDS_NAMESPACE;

namespace tests
{
	TEST_CLASS(RedBlackTreeTests)
	{
		TEST_METHOD(RedBlack_Colour_Arithmetic_Correc)
		{
			using detail::NodeColour;
			Assert::IsTrue(NodeColour::Black == NodeColour::Red + NodeColour::Black);
			Assert::IsTrue(NodeColour::NegativeBlack + NodeColour::Black == NodeColour::Red);
			Assert::IsTrue(NodeColour::Black + NodeColour::Black == NodeColour::DoubleBlack);

			Assert::IsTrue(NodeColour::Red - NodeColour::Black == NodeColour::NegativeBlack);
			Assert::IsTrue(NodeColour::Black - NodeColour::Black == NodeColour::Red);
			Assert::IsTrue(NodeColour::DoubleBlack - NodeColour::Black == NodeColour::Black);
		}

		TEST_METHOD(MakeNullRedblackNode_Makes_Node_Of_Correct_Colour)
		{
			using detail::NodeColour;

			Assert::IsTrue(NodeColour::Red == colour(detail::make_null_redblack_node<int>(NodeColour::Red)));
			Assert::IsTrue(NodeColour::Black == colour(detail::make_null_redblack_node<int>(NodeColour::Black)));
			Assert::IsTrue(NodeColour::DoubleBlack == colour(detail::make_null_redblack_node<int>(NodeColour::DoubleBlack)));
			Assert::IsTrue(NodeColour::NegativeBlack == colour(detail::make_null_redblack_node<int>(NodeColour::NegativeBlack)));
		}

		TEST_METHOD(MakeNullRedblackNode_Creates_Falsy_Node)
		{
			Assert::IsFalse(static_cast<bool>(detail::make_null_redblack_node<int>()));
		}

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

		TEST_METHOD(RedBlackTree_Inserting_Element_Returns_Inserted_True)
		{
			using std::get;

			const redblack_tree<int> tree;

			auto inserted = get<2>(tree.insert(5));

			Assert::IsTrue(inserted);
		}

		TEST_METHOD(RedBlackTree_Inserting_Element_Returns_Iterator_To_Element)
		{
			using std::get;

		    redblack_tree<int> tree;
			redblack_tree<int>::iterator inserted;

			std::tie(tree, inserted, std::ignore) = tree.insert(5);

			Assert::AreEqual(5, *inserted);
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

		void TestInsert(std::vector<int> values, bool verifyIterator = false)
		{
            redblack_tree<int> tree;
			redblack_tree<int>::iterator it;
			bool inserted;

			for (int i : values)
			{
				std::tie(tree, it, inserted) = tree.insert(i);

				if (verifyIterator)
				{
				    Assert::AreEqual(i, *it);
				    Assert::IsTrue(inserted);
				}
			}

			for (int i : values)
			{
				auto found = tree.find(i);

				Assert::IsTrue(found != tree.end());
				Assert::AreEqual(i, *found);
			}
		}

		void TestInsert(int count, bool verifyIterator = false)
		{
			std::vector<int> data(count);

			for (int i = 0; i < count; i++)
			{
				data[i] = i;
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

		TEST_METHOD(RedBlackTree_Insert_Returns_Correct_Iterator_2)
		{
			TestInsert(2, true);
		}

		TEST_METHOD(RedBlackTree_Insert_Returns_Correct_Iterator_3)
		{
			TestInsert(3, true);
		}

		TEST_METHOD(RedBlackTree_Insert_Returns_Correct_Iterator_10)
		{
			TestInsert(10, true);
		}

		TEST_METHOD(RedBlackTree_Insert_Correct_For_Non_Monotonic_Elements)
		{
			std::vector<int> data{ 1, 3, 5, 2, 4, 0, -5 };

			TestInsert(data, true);
		}
	};
}