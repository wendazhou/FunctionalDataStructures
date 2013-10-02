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
		}
	};
}