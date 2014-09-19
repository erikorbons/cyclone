#include <boost/test/unit_test.hpp>

#include <iostream>
#include <cyclone/core/TextBuffer.h>
#include <cyclone/parser/Lexer.h>

using namespace cyclone::core;
using namespace cyclone::parser;
using namespace cyclone::syntaxtree;

Lexer mkLexer (const std::u16string & value) {
	TextBuffer buffer (value);
	return Lexer (buffer, buffer.begin (), buffer.end ());
}

BOOST_AUTO_TEST_SUITE (TestLexer)

BOOST_AUTO_TEST_CASE (testCreate) {
	TextBuffer buffer (u"a b c d e");
	Lexer lexer (buffer, buffer.begin (), buffer.end ());
}

BOOST_AUTO_TEST_CASE (testEndOfInput) {
	Lexer l = mkLexer (u"");

	BOOST_CHECK (l.check (TokenType::END_OF_INPUT) && l.la ().length () == 0);
}

BOOST_AUTO_TEST_CASE (testWhitespace) {
	Lexer l = mkLexer (u"    \n\t    ");

	BOOST_CHECK (l.check (TokenType::WHITESPACE) && l.la ().length () == 10);
}

BOOST_AUTO_TEST_CASE (testSingleLineComment) {
	Lexer l = mkLexer (u"// Hello, World!");

	BOOST_CHECK (l.check (TokenType::SINGLE_LINE_COMMENT) && l.la ().length () == 16);
}

BOOST_AUTO_TEST_SUITE_END ()
