#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE	Cyclone Parser
#include <boost/test/unit_test.hpp>

#include <cyclone/core/TextBuffer.h>
#include <cyclone/parser/Lexer.h>
#include <cyclone/parser/Parser.h>

using namespace cyclone::core;
using namespace cyclone::syntaxtree;
using namespace cyclone::parser;

static Lexer mkLexer (const std::u16string & value) {
	TextBuffer buffer (value);
	return Lexer (buffer, buffer.begin (), buffer.end ());
}

BOOST_AUTO_TEST_SUITE(TestParser)

BOOST_AUTO_TEST_CASE (my_test) {
	BOOST_TEST_MESSAGE ("Hello, World!");
	BOOST_CHECK (1 == 1);
}

BOOST_AUTO_TEST_CASE (testCreateParser) {
	Lexer l = mkLexer (u"");
	Parser p (l);
}

BOOST_AUTO_TEST_CASE (testParseCompilationUnit) {
	Lexer l = mkLexer (u"  ");
	Parser p (l);

	std::shared_ptr<Node> cu = p.parseCompilationUnit ();

	BOOST_CHECK (cu->nodeCount () == 2);
	BOOST_CHECK (cu->length () == 2);
}

BOOST_AUTO_TEST_SUITE_END()
