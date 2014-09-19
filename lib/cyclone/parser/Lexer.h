#ifndef CYCLONE_PARSER_LEXER_H__
#define CYCLONE_PARSER_LEXER_H__

#include <cyclone/core/TextBuffer.h>
#include <cyclone/syntaxtree/Token.h>
#include <vector>

namespace cyclone {
namespace parser {

namespace internal {
	class Scanner {
	public:
		typedef cyclone::core::TextBuffer			TextBuffer;
		typedef cyclone::core::TextBuffer::Iterator TextIterator;

		Scanner (const TextBuffer buffer, const TextIterator rangeBegin, const TextIterator rangeEnd)
			: m_textBuffer (buffer),
			  m_bufferEnd (m_textBuffer.end ()),
			  m_rangeBegin (m_textBuffer.at (rangeBegin.offset ())),
			  m_rangeEnd (m_textBuffer.at (rangeEnd.offset ())),
			  m_current (m_textBuffer.at (rangeBegin.offset ())) {
		}

		Scanner (const Scanner & other)
			: m_textBuffer (other.m_textBuffer),
			  m_bufferEnd (m_textBuffer.at (other.m_bufferEnd.offset ())),
			  m_rangeBegin (m_textBuffer.at (other.m_rangeBegin.offset ())),
			  m_rangeEnd (m_textBuffer.at (other.m_rangeEnd.offset ())),
			  m_current (m_textBuffer.at (other.m_current.offset ())) {
		}

		Scanner & operator = (const Scanner & other) {
			m_textBuffer = other.m_textBuffer;
			m_bufferEnd = m_textBuffer.at (other.m_bufferEnd.offset ());
			m_rangeBegin = m_textBuffer.at (other.m_rangeBegin.offset ());
			m_rangeEnd = m_textBuffer.at (other.m_rangeEnd.offset ());
			m_current = m_textBuffer.at (other.m_current.offset ());
			return *this;
		}

		char16_t la (unsigned offset = 0) {
			while (m_lookahead.size () <= offset) {
				if (m_current == m_bufferEnd) {
					m_lookahead.push_back (0);
				} else {
					m_lookahead.push_back (*m_current);
					++ m_current;
				}
			}

			return m_lookahead[offset];
		}

		char16_t accept () {
			// Make sure there is at least one character to accept:
			la ();

			// Erase the first character:
			char16_t v = m_lookahead[0];
			m_lookahead.erase (m_lookahead.begin ());
			return v;
		}

		bool check (char16_t ch, unsigned offset = 0) {
			return la (offset) == ch;
		}

		bool isRangeComplete () const {
			return m_current >= m_rangeEnd;
		}

	private:

		TextBuffer				m_textBuffer;
		TextIterator			m_bufferEnd;
		TextIterator			m_rangeBegin;
		TextIterator			m_rangeEnd;
		TextIterator			m_current;
		std::vector<char16_t>	m_lookahead;
	};
}

class Lexer {
public:

	typedef cyclone::core::TextBuffer			TextBuffer;
	typedef cyclone::core::TextBuffer::Iterator TextIterator;
	typedef cyclone::syntaxtree::Token			Token;
	typedef cyclone::syntaxtree::TokenType		TokenType;

	Lexer (const TextBuffer textBuffer, const TextIterator rangeBegin, const TextIterator rangeEnd)
		: m_scanner (textBuffer, rangeBegin, rangeEnd) {
	}

	Lexer (const Lexer & other) : m_scanner (other.m_scanner) {
	}

	Lexer & operator = (const Lexer & other) {
		m_scanner = other.m_scanner;
		return *this;
	}

	Token la (unsigned offset = 0);
	void accept ();

	bool check (TokenType tokenType, unsigned offset = 0) {
		return la (offset).type () == tokenType;
	}

private:

	void next ();
	void emit (const Token & token);

	Token parseString (TokenType tokenType);
	Token parseDecimal ();
	Token parseHex ();

	internal::Scanner	m_scanner;
	std::vector<Token>	m_lookahead;
};

}
}
#endif	// CYCLONE_PARSER_LEXER_H__
