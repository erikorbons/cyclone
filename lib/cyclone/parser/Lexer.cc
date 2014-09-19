#include <cyclone/parser/Lexer.h>

namespace cyclone {
namespace parser {

	using namespace cyclone::core;
	using namespace cyclone::syntaxtree;
	using namespace cyclone::parser;

	bool isDigit (char16_t c) {
		return c >= '0' && c <= '9';
	}

	bool isAlpha (char16_t c) {
		return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
	}

	struct StringTokenPair {
		std::u16string	name;
		TokenType		tokenType;
	};

	StringTokenPair keywords[] = {
		{ u"namespace", TokenType::NAMESPACE },
		{ u"using", TokenType::USING }
	};

	StringTokenPair punctuation[] = {
		{ u"~=", TokenType::INVERT_ASSIGN },
		{ u"~", TokenType::INVERT },
		{ u"!=", TokenType::NOT_EQUAL },
		{ u"!", TokenType::NOT },
		{ u"%=", TokenType::MODULO_ASSIGN },
		{ u"%", TokenType::MODULO },
		{ u"^=", TokenType::BIT_XOR_ASSIGN },
		{ u"^^", TokenType::LOGICAL_XOR },
		{ u"^", TokenType::BIT_XOR },
		{ u"&=", TokenType::BIT_AND_ASSIGN },
		{ u"&&", TokenType::LOGICAL_AND },
		{ u"&", TokenType::BIT_AND },
		{ u"*=", TokenType::MULTIPLY_ASSIGN },
		{ u"*", TokenType::MULTIPLY },
		{ u"-=", TokenType::MINUS_ASSIGN },
		{ u"--", TokenType::DECREMENT },
		{ u"-", TokenType::MINUS },
		{ u"+=", TokenType::PLUS_ASSIGN },
		{ u"++", TokenType::INCREMENT },
		{ u"+", TokenType::PLUS },
		{ u"==", TokenType::EQUAL },
		{ u"=", TokenType::ASSIGN },
		{ u":", TokenType::COLON },
		{ u";", TokenType::SEMICOLON },
		{ u"||", TokenType::LOGICAL_OR },
		{ u"|=", TokenType::OR_ASSIGN },
		{ u"|", TokenType::BIT_OR },
		{ u",", TokenType::COMMA },
		{ u".", TokenType::DOT },
		{ u"/=", TokenType::DIVIDE_ASSIGN },
		{ u"/", TokenType::DIVIDE },
		{ u"?", TokenType::QUESTION_MARK },
		{ u"<=", TokenType::LESS_THAN_EQUAL },
		{ u">=", TokenType::GREATER_THAN_EQUAL },
		{ u"<", TokenType::LESS_THAN },
		{ u">", TokenType::GREATER_THAN }
	};

	TokenType findKeyword (const std::u16string & name) {
		for (unsigned i = 0; i < sizeof (keywords) / sizeof (StringTokenPair); ++ i) {
			if (keywords[i].name == name) {
				return keywords[i].tokenType;
			}
		}

		return TokenType::NAME;
	}

	TokenType findPunctuation (const std::u16string & value) {
		for (unsigned i = 0; i < sizeof (punctuation) / sizeof (StringTokenPair); ++ i) {
			if (punctuation[i].name == value) {
				return punctuation[i].tokenType;
			}
		}

		return TokenType::END_OF_INPUT;
	}

	Lexer::Token Lexer :: la (unsigned offset) {
		while (m_lookahead.size () <= offset) {
			next ();
		}

		return m_lookahead[offset];
	}

	void Lexer :: accept () {
		// Make sure there is a token in the lookahead list:
		la (0);

		// Remove the first token from the lookahead list:
		m_lookahead.erase (m_lookahead.begin ());
	}

	void Lexer :: next () {
		// Parse EOF:
		if (m_scanner.isRangeComplete ()) {
			emit (Token (TokenType::END_OF_INPUT, 0));
			return;
		}

		// Parse whitespace:
		if (m_scanner.la () > 0 && m_scanner.la () <= ' ') {
			std::size_t length = 0;
			while (m_scanner.la () > 0 && m_scanner.la () <= ' ') {
				++ length;
				m_scanner.accept ();
			}
			emit (Token (TokenType::WHITESPACE, length));
			return;
		}

		// Parse single line comments:
		if (m_scanner.la (0) == '/' && m_scanner.la (1) == '/') {
			std::size_t length = 0;
			while (m_scanner.la () != 0 && m_scanner.la () != '\r' && m_scanner.la () != '\n') {
				++ length;
				m_scanner.accept ();
			}

			if (m_scanner.la () == '\r' && m_scanner.la (1) == '\n') {
				length += 2;
				m_scanner.accept ();
				m_scanner.accept ();
			} else if (m_scanner.la () == '\r' || m_scanner.la () == '\n') {
				length ++;
				m_scanner.accept ();
			}

			emit (Token (TokenType::SINGLE_LINE_COMMENT, length));
			return;
		}

		// Parse multi-line comments:
		if (m_scanner.la (0) == '/' && m_scanner.la (1) == '*') {
			std::size_t length = 0;

			while (m_scanner.la () != 0 && !(length >= 2 && m_scanner.la (0) == '*' && m_scanner.la (1) == '/')) {
				++ length;
				m_scanner.accept ();
			}

			if (m_scanner.la () == 0) {
				// Unterminated comment:
				emit (Token (TokenType::MULTI_LINE_COMMENT, TokenError::UNTERMINATED_COMMENT, length, 0));
				return;
			}

			length += 2;
			m_scanner.accept ();
			m_scanner.accept ();

			emit (Token (TokenType::MULTI_LINE_COMMENT, length));
			return;
		}

		// Parse strings:
		if (m_scanner.la (0) == '\"') {
			emit (parseString (TokenType::STRING_CONSTANT));
			return;
		}

		// Parse characters:
		if (m_scanner.la (0) == '\'') {
			emit (parseString (TokenType::CHARACTER_CONSTANT));
			return;
		}

		// Parse decimal numbers:

		// Parse hex numbers:

		// Parse quoted names:
		if (m_scanner.la (0) == '`') {
			emit (parseString (TokenType::NAME));
			return;
		}

		// Parse names and keywords:
		if (m_scanner.la (0) == '_' || m_scanner.la (0) == '$' || isAlpha (m_scanner.la (0))) {
			std::size_t length = 0;
			std::u16string value;

			while (m_scanner.la () == '_'
					|| m_scanner.la () == '$'
					|| isAlpha (m_scanner.la ())
					|| isDigit (m_scanner.la ())) {
				++ length;
				value += m_scanner.accept ();
			}

			emit (Token (findKeyword (value), length));
			return;
		}

		// Parse punctuation:
		{
			std::size_t length = 0;
			std::u16string value;
			TokenType punctuationToken;
			while (m_scanner.la () > 0) {
				TokenType t;
				value += m_scanner.la ();
				if ((t = findPunctuation (value)) == TokenType::END_OF_INPUT) {
					break;
				}

				++ length;
				m_scanner.accept ();
				punctuationToken = t;
			}
			if (length > 0) {
				emit (Token (punctuationToken, length));
				return;
			}
		}

		// Parse invalid characters:
		{
			std::size_t length = 0;
			while (m_scanner.la () > ' ') {
				++ length;
				m_scanner.accept ();
			}

			emit (Token (TokenType::INVALID_CHARACTERS, TokenError::UNEXPECTED_CHARACTERS, length, 0));
			return;
		}
	}

	void Lexer :: emit (const Token & token) {
		m_lookahead.push_back (token);
	}

	Token Lexer :: parseString (TokenType tokenType) {
		char16_t quoteCharacter = m_scanner.la ();
		std::size_t length = 1;
		m_scanner.accept ();
		bool multipleCharacters = false;

		while (m_scanner.la () != quoteCharacter) {
			if (tokenType == TokenType::CHARACTER_CONSTANT && length > 1) {
				multipleCharacters = true;
			}

			if (m_scanner.la () == 0 || m_scanner.la () == '\r' || m_scanner.la () == '\n') {
				// Unterminated string:
				return Token (tokenType, TokenError::UNTERMINATED_CHARACTER_SEQUENCE, length, 0);
			} else if (m_scanner.la () == '\\') {
				// Escape sequence:
				char16_t c = m_scanner.la (1);
				if (c == quoteCharacter || c == '\\' || c == 'n' || c == 'r'
						|| c == 't') {
					length += 2;
					m_scanner.accept ();
					m_scanner.accept ();
					continue;
				} else if (c == 'x'
						&& isDigit (m_scanner.la (2))
						&& isDigit (m_scanner.la (3))
						&& isDigit (m_scanner.la (4))
						&& isDigit (m_scanner.la (5))) {
					length += 6;
					for (int i = 0; i < 6; ++ i) { m_scanner.accept (); }
					continue;
				}
			}

			++ length;
			m_scanner.accept ();
		}

		++ length;
		m_scanner.accept ();

		// Report invalid characters when multiple characters have been parsed between the
		// quotes:
		if (tokenType == TokenType::CHARACTER_CONSTANT && multipleCharacters) {
			return Token (tokenType, TokenError::INVALID_CHARACTER_CONSTANT, length, 0);
		}

		return Token (tokenType, length);
	}

}	// namespace parser
}	// namespace cyclone
