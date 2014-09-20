#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE	Cyclone Parser
#include <boost/test/unit_test.hpp>

#include <cyclone/parser/Parser.h>

using namespace cyclone::syntaxtree;
using namespace cyclone::parser;

BOOST_AUTO_TEST_SUITE(TestParser)

BOOST_AUTO_TEST_CASE (my_test) {
	BOOST_TEST_MESSAGE ("Hello, World!");
	BOOST_CHECK (1 == 1);
}

BOOST_AUTO_TEST_CASE (testCreateRules) {
	auto tokenRule = token (TokenType::NAME);

	auto optionalRule = token (TokenType::NAME) (0, 1);

	auto concatenatedRule = token (TokenType::NAME) + token (TokenType::STRING_CONSTANT);

	auto alternateRule = tokenRule | concatenatedRule | optionalRule;

	auto namedRule = rule<Node> (u"name") > alternateRule;
}

BOOST_AUTO_TEST_CASE (testCreateParser) {
	auto p = parser (rule<Node> (u"testRule") > token (TokenType::NAME));
}

BOOST_AUTO_TEST_SUITE_END()
