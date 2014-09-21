#include <cyclone/parser/Parser.h>

namespace cyclone {
namespace parser {

	namespace internal {
		RuleScope :: RuleScope (Parser & parser, NodeType nodeType)
			: m_parser (parser),
			  m_target (std::make_shared<Node> (nodeType)) {
			m_parser.m_scopes.push (this);
		}

		RuleScope :: ~RuleScope () {
			m_parser.m_scopes.pop ();
		}

	}

	Parser :: Parser (Lexer & lexer) : m_lexer (lexer) {
	}

	Parser::Result<Parser::Node> Parser :: parseCompilationUnit () {
		RuleScope scope (*this, NodeType::COMPILATION_UNIT);

		while (!check (TokenType::END_OF_INPUT)) {
			parseNamespaceMember (false);
		}

		// Accept the end of input token:
		accept ();

		return scope.get ();
	}

	Parser::Result<Parser::Node> Parser :: parseNamespace () {
		RuleScope scope (*this, NodeType::NAMESPACE);

		return scope.get ();
	}

	Parser::Result<Parser::Node> Parser :: parseUsing () {
		RuleScope scope (*this, NodeType::USING);

		return scope.get ();
	}

	void Parser :: parseNamespaceMember (bool inBlock) {
		if (check (TokenType::USING)) {
			add (parseUsing ());
		} else if (check (TokenType::NAMESPACE)) {
			add (parseNamespace ());
		} else {
			// Recover until the next valid token is found:
			if (inBlock) {
				add (recover ({ TokenType::USING, TokenType::NAMESPACE, TokenType::RIGHT_CURLY }));
			} else {
				add (recover ({ TokenType::USING, TokenType::NAMESPACE }));
			}
		}
	}

	bool Parser :: isNonSignificant (TokenType tokenType) {
		return tokenType == TokenType :: WHITESPACE
			|| tokenType == TokenType :: SINGLE_LINE_COMMENT
			|| tokenType == TokenType :: MULTI_LINE_COMMENT
			|| tokenType == TokenType :: INVALID_CHARACTERS;
	}

	Parser::Node * Parser :: node () const {
		return m_scopes.top ()->node ();
	}

	bool Parser :: check (TokenType tokenType) {
		// See if the next significant token is of the given type:
		unsigned i = 0;

		while (isNonSignificant (m_lexer.la (i).type ())) {
			++ i;
		}

		return m_lexer.la (i).type () == tokenType;
	}

	void Parser :: expect (TokenType tokenType) {

	}

	void Parser :: accept () {
		// Accept all tokens up to and including the next significant token:
		while (true) {
			Token token = m_lexer.accept ();
			node ()->addNode (std::make_shared<Terminal> (token));
			if (!isNonSignificant (token.type ())) {
				break;
			}
		}
	}

	Parser::Result<Parser::Node> Parser :: recover (std::initializer_list<TokenType> validTokenTypes) {
		RuleScope scope (*this, NodeType::ERROR);

		while (true) {
			if (check (TokenType::END_OF_INPUT)) {
				return scope.get ();
			}

			for (TokenType tt: validTokenTypes) {
				if (check (tt)) {
					return scope.get ();
				}
			}

			accept ();
		}
	}
}
}
