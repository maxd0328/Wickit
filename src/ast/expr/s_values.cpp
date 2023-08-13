#include "ast/expr/s_values.h"
#include "ast/expr/s_helpers.h"

using namespace wckt;
using namespace wckt::ast;
using namespace wckt::build;

S_Expression::S_Expression(bool pop)
: ASTNode("expression"), pop(pop) {}

void S_Expression::parse(Parser& parser)
{
	parser.match<SH_Expression0>();
}
