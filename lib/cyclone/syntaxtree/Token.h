#ifndef CYCLONE_SYNTAXTREE_TOKEN_H__
#define CYCLONE_SYNTAXTREE_TOKEN_H__

namespace cyclone {
namespace syntaxtree {

enum class TokenType : unsigned char {
	END_OF_INPUT,
	INVALID_CHARACTERS,		// Unparseable characters are reported as a special token.

	// Whitespace and comments:
	WHITESPACE,
	SINGLE_LINE_COMMENT,
	MULTI_LINE_COMMENT,

	// Constants:
	STRING_CONSTANT,		// Quoted strings
	CHARACTER_CONSTANT,		// Quoted character constant
	DECIMAL_CONSTANT,		// Decimal (byte, int, long, float, double, etc.)
	HEX_CONSTANT,			// Hex (byte, int, long, etc.)

	NAME,					// A name, either ordinary name or quoted using backticks.

	// Keywords:
	NAMESPACE,
	USING,

	// Punctuation:
	LEFT_BRACE,				// (
	RIGHT_BRACE,			// )
	LEFT_CURLY,				// {
	RIGHT_CURLY,			// }
	LEFT_BRACKET,			// [
	RIGHT_BRACKET,			// ]

	INVERT,					// ~
	INVERT_ASSIGN,			// ~=
	NOT,					// !
	MODULO,					// %
	MODULO_ASSIGN,			// %=
	BIT_XOR,				// ^
	BIT_XOR_ASSIGN,			// ^=
	LOGICAL_XOR,			// ^^
	BIT_AND,				// &
	LOGICAL_AND,			// &&
	BIT_AND_ASSIGN,			// &=
	MULTIPLY,				// *
	MULTIPLY_ASSIGN,		// *=
	MINUS,					// -
	MINUS_ASSIGN,			// -=
	DECREMENT,				// --
	PLUS,					// +
	PLUS_ASSIGN,			// +=
	INCREMENT,				// ++
	ASSIGN,					// =
	COLON,					// :
	SEMICOLON,				// ;
	BIT_OR,					// |
	LOGICAL_OR,				// ||
	OR_ASSIGN,				// |=
	COMMA,					// ,
	DOT,					// .
	DIVIDE,					// /
	DIVIDE_ASSIGN,			// /=
	QUESTION_MARK,			// ?
	LESS_THAN,				// <
	GREATER_THAN,			// >
	LESS_THAN_EQUAL,		// <=
	GREATER_THAN_EQUAL,		// >=
	NOT_EQUAL,				// !=
	EQUAL					// ==
};

enum class TokenError : unsigned char {
	NO_ERROR = 0,

	UNEXPECTED_CHARACTERS = 1,
	UNTERMINATED_COMMENT = 2,
	UNTERMINATED_CHARACTER_SEQUENCE = 3,	// Unterminated string, character or quoted name.
	INVALID_CHARACTER_CONSTANT = 4,			// Character too long.
	INVALID_NUMBER_FORMAT = 5
};

class Token {
public:

	Token (TokenType type, std::size_t length)
		: m_type (type), m_error (TokenError::NO_ERROR), m_length (length), m_errorOffset (0) {
	}

	Token (TokenType type, TokenError error, std::size_t length, std::size_t errorOffset)
		: m_type (type), m_error (error), m_length (length), m_errorOffset (errorOffset) {
	}

	TokenType type () const {
		return m_type;
	}

	TokenError error () const {
		return m_error;
	}

	std::size_t length () const {
		return m_length;
	}

	std::size_t errorOffset () const {
		return m_errorOffset;
	}

private:

	TokenType	m_type;
	TokenError	m_error;
	std::size_t	m_length;
	std::size_t	m_errorOffset;
};

}	// namespace syntaxtree
}	// namespace cyclone

#endif	// CYCLONE_SYNTAXTREE_TOKEN_H__
