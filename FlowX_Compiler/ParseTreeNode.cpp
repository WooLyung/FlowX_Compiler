#include "pch.h"
#include "ParseTreeNode.h"

namespace flowx::parser
{
	const SourceLocation& ParseTreeNode::GetLocation() const
	{
		return location_;
	}
}