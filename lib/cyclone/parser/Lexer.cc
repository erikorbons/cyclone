#include <cyclone/parser/Lexer.h>

namespace cyclone {
namespace parser {

	using namespace cyclone::core;
	using namespace cyclone::syntaxtree;
	using namespace cyclone::parser;

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

		// Parse characters:

		// Parse decimal numbers:

		// Parse hex numbers:

		// Parse quoted names:

		// Parse names and keywords:

		// Parse punctuation:

		// Parse invalid characters:
	}

	void Lexer :: emit (const Token & token) {
		m_lookahead.push_back (token);
	}

}	// namespace parser
}	// namespace cyclone
