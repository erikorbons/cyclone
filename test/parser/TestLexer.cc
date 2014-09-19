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
	{
		Lexer l = mkLexer (u"// Hello, World!");

		BOOST_CHECK (l.check (TokenType::SINGLE_LINE_COMMENT) && l.la ().length () == 16);
		BOOST_CHECK (l.check (TokenType::END_OF_INPUT, 1));
	}

	{
		Lexer l = mkLexer (u"// Hello, World!\n  ");

		BOOST_CHECK (l.check (TokenType::SINGLE_LINE_COMMENT) && l.la ().length () == 17);
		BOOST_CHECK (l.check (TokenType::WHITESPACE, 1) && l.la (1).length () == 2);
		BOOST_CHECK (l.check (TokenType::END_OF_INPUT, 2));
	}
}

BOOST_AUTO_TEST_CASE (testMultiLineComment) {
	{
		Lexer l = mkLexer (u"/* Hello, World!\nabcd*/");

		BOOST_CHECK (l.check (TokenType::MULTI_LINE_COMMENT));
		BOOST_CHECK (l.la ().length () == 23);
	}

	{
		Lexer l = mkLexer (u"  /* Hello, World!\nabcd*/  ");

		BOOST_CHECK (l.check (TokenType::WHITESPACE));
		BOOST_CHECK (l.la ().length () == 2);
		BOOST_CHECK (l.check (TokenType::MULTI_LINE_COMMENT, 1));
		BOOST_CHECK (l.la (1).length () == 23);
		BOOST_CHECK (l.check (TokenType::WHITESPACE, 2));
		BOOST_CHECK (l.la (2).length () == 2);
	}
}

BOOST_AUTO_TEST_CASE (testString) {
	{
		Lexer l = mkLexer (u"\"Hello, World!\"");

		BOOST_CHECK (l.check (TokenType::STRING_CONSTANT));
		BOOST_CHECK (l.la ().length () == 15);
	}
	{
		Lexer l = mkLexer (u"\"Hello, World!\\\"\"");

		BOOST_CHECK (l.check (TokenType::STRING_CONSTANT));
		BOOST_CHECK (l.la ().length () == 17);
	}
	{
		Lexer l = mkLexer (u"  \"Hello, World!\"  ");

		BOOST_CHECK (l.check (TokenType::WHITESPACE, 0));
		BOOST_CHECK (l.la (0).length () == 2);
		BOOST_CHECK (l.check (TokenType::STRING_CONSTANT, 1));
		BOOST_CHECK (l.la (1).length () == 15);
		BOOST_CHECK (l.check (TokenType::WHITESPACE, 2));
		BOOST_CHECK (l.la (2).length () == 2);
	}
}

BOOST_AUTO_TEST_CASE (testCharacter) {
	{
		Lexer l = mkLexer (u"  \'a\'\'\\\'\'  ");

		BOOST_CHECK (l.check (TokenType::WHITESPACE, 0));
		BOOST_CHECK (l.la (0).length () == 2);
		BOOST_CHECK (l.check (TokenType::CHARACTER_CONSTANT, 1));
		BOOST_CHECK (l.la (1).length () == 3);
		BOOST_CHECK (l.check (TokenType::CHARACTER_CONSTANT, 2));
		BOOST_CHECK (l.la (2).length () == 4);
		BOOST_CHECK (l.check (TokenType::WHITESPACE, 3));
		BOOST_CHECK (l.la (3).length () == 2);
	}
}

BOOST_AUTO_TEST_CASE (testQuotedName) {
	Lexer l = mkLexer (u"  `Hello, World!`  `\\``  ");

	BOOST_CHECK (l.check (TokenType::WHITESPACE, 0));
	BOOST_CHECK (l.la (0).length () == 2);
	BOOST_CHECK (l.check (TokenType::NAME, 1));
	BOOST_CHECK (l.la (1).length () == 15);
	BOOST_CHECK (l.check (TokenType::WHITESPACE, 2));
	BOOST_CHECK (l.la (2).length () == 2);
	BOOST_CHECK (l.check (TokenType::NAME, 3));
	BOOST_CHECK (l.la (3).length () == 4);
	BOOST_CHECK (l.check (TokenType::WHITESPACE, 4));
	BOOST_CHECK (l.la (4).length () == 2);
}

BOOST_AUTO_TEST_CASE (testKeyword) {
	Lexer l = mkLexer (u"  namespace  using  ");

	BOOST_CHECK (l.check (TokenType::WHITESPACE, 0));
	BOOST_CHECK (l.la (0).length () == 2);
	BOOST_CHECK (l.check (TokenType::NAMESPACE, 1));
	BOOST_CHECK (l.la (1).length () == 9);
	BOOST_CHECK (l.check (TokenType::WHITESPACE, 2));
	BOOST_CHECK (l.la (2).length () == 2);
	BOOST_CHECK (l.check (TokenType::USING, 3));
	BOOST_CHECK (l.la (3).length () == 5);
	BOOST_CHECK (l.check (TokenType::WHITESPACE, 4));
	BOOST_CHECK (l.la (4).length () == 2);
}

BOOST_AUTO_TEST_CASE (testName) {
	Lexer l = mkLexer (u"  _abcd01  $efgh1  ");

	BOOST_CHECK (l.check (TokenType::WHITESPACE, 0));
	BOOST_CHECK (l.la (0).length () == 2);
	BOOST_CHECK (l.check (TokenType::NAME, 1));
	BOOST_CHECK (l.la (1).length () == 7);
	BOOST_CHECK (l.check (TokenType::WHITESPACE, 2));
	BOOST_CHECK (l.la (2).length () == 2);
	BOOST_CHECK (l.check (TokenType::NAME, 3));
	BOOST_CHECK (l.la (3).length () == 6);
	BOOST_CHECK (l.check (TokenType::WHITESPACE, 4));
	BOOST_CHECK (l.la (4).length () == 2);
}

BOOST_AUTO_TEST_CASE (testPunctuation) {
	Lexer l = mkLexer (u"  ++ - >=/  ");

	BOOST_CHECK (l.check (TokenType::WHITESPACE, 0));
	BOOST_CHECK (l.la (0).length () == 2);
	BOOST_CHECK (l.check (TokenType::INCREMENT, 1));
	BOOST_CHECK (l.la (1).length () == 2);
	BOOST_CHECK (l.check (TokenType::WHITESPACE, 2));
	BOOST_CHECK (l.la (2).length () == 1);
	BOOST_CHECK (l.check (TokenType::MINUS, 3));
	BOOST_CHECK (l.la (3).length () == 1);
	BOOST_CHECK (l.check (TokenType::WHITESPACE, 4));
	BOOST_CHECK (l.la (4).length () == 1);
	BOOST_CHECK (l.check (TokenType::GREATER_THAN_EQUAL, 5));
	BOOST_CHECK (l.la (5).length () == 2);
	BOOST_CHECK (l.check (TokenType::DIVIDE, 6));
	BOOST_CHECK (l.la (6).length () == 1);
	BOOST_CHECK (l.check (TokenType::WHITESPACE, 7));
	BOOST_CHECK (l.la (7).length () == 2);
}

BOOST_AUTO_TEST_CASE (testDecimal) {
	BOOST_CHECK (false);
}

BOOST_AUTO_TEST_CASE (testHex) {
	BOOST_CHECK (false);
}


BOOST_AUTO_TEST_CASE (testErrorInvalidCharacters) {
	Lexer l = mkLexer (u"  @abcde  ");

	BOOST_CHECK (l.check (TokenType::WHITESPACE, 0));
	BOOST_CHECK (l.la (0).length () == 2);
	BOOST_CHECK (l.check (TokenType::INVALID_CHARACTERS, 1));
	BOOST_CHECK (l.la (1).length () == 6);
	BOOST_CHECK (l.la (1).error () == TokenError::UNEXPECTED_CHARACTERS);
	BOOST_CHECK (l.check (TokenType::WHITESPACE, 2));
	BOOST_CHECK (l.la (2).length () == 2);
}

BOOST_AUTO_TEST_CASE (testErrorUnterminatedComment) {
	Lexer l = mkLexer (u"/* \n\n\nabcd\n");

	BOOST_CHECK (l.check (TokenType::MULTI_LINE_COMMENT, 0));
	BOOST_CHECK (l.la (0).length () == 11);
	BOOST_CHECK (l.la (0).error () == TokenError::UNTERMINATED_COMMENT);
}

BOOST_AUTO_TEST_CASE (testErrorUnterminatedCharacterSequence) {
	Lexer l = mkLexer (u"\"abcd\nabcd");

	BOOST_CHECK (l.check (TokenType::STRING_CONSTANT, 0));
	BOOST_CHECK (l.la (0).length () == 5);
	BOOST_CHECK (l.la (0).error () == TokenError::UNTERMINATED_CHARACTER_SEQUENCE);
	BOOST_CHECK (l.check (TokenType::WHITESPACE, 1));
	BOOST_CHECK (l.la (1).length () == 1);
	BOOST_CHECK (l.check (TokenType::NAME, 2));
	BOOST_CHECK (l.la (2).length () == 4);
}

BOOST_AUTO_TEST_CASE (testErrorInvalidCharacterConstant) {
	Lexer l = mkLexer (u"\'ab\'");

	BOOST_CHECK (l.check (TokenType::CHARACTER_CONSTANT, 0));
	BOOST_CHECK (l.la (0).length () == 4);
	BOOST_CHECK (l.la (0).error () == TokenError::INVALID_CHARACTER_CONSTANT);
}

BOOST_AUTO_TEST_SUITE_END ()
