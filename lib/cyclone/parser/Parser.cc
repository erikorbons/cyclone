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

	Parser::Result Parser :: parseCompilationUnit () {
		RuleScope scope (*this, NodeType::COMPILATION_UNIT);

		parseNamespaceContent ();

		// Accept the end of input token:
		expect (TokenType :: END_OF_INPUT);

		return scope.get ();
	}

	Parser::Result Parser :: parseNamespace () {
		RuleScope scope (*this, NodeType::NAMESPACE);

		expect (TokenType::NAMESPACE);

		add (parseScopedName ());

		expect (TokenType::LEFT_CURLY);

		parseNamespaceContent ();

		expect (TokenType :: RIGHT_CURLY);

		return scope.get ();
	}

	Parser::Result Parser :: parseUsing () {
		RuleScope scope (*this, NodeType::USING);

		expect (TokenType::USING);

		add (parseScopedName ());

		if (check (TokenType::ASSIGN)) {
			accept ();

			expect (TokenType::NAME);
		}

		expect (TokenType::SEMICOLON);

		return scope.get ();
	}

	Parser::Result Parser :: parseScopedName () {
		RuleScope scope (*this, NodeType::SCOPED_NAME);

		expect (TokenType::NAME);

		while (check (TokenType::DOT)) {
			accept ();
			expect (TokenType::NAME);
		}

		return scope.get ();
	}


	void Parser :: parseNamespaceContent () {
		while (true) {
			if (check (TokenType::USING)) {
				add (parseUsing ());
			} else if (check (TokenType::NAMESPACE)) {
				add (parseNamespace ());
			} else {
				break;
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
		int n = 0;
		while (!check (tokenType)) {
			// Emit an error node:

			// Break at end of input:
			if (check (TokenType::END_OF_INPUT)) {
				add (recover ({ }));
				return;
			}

			// Break at a semicolon:
			if (check (TokenType::SEMICOLON)) {
				add (recover ({ TokenType::SEMICOLON }));
				return;
			}

			// Break after parsing 50 tokens:
			if (n >= 50) {
				add (std::make_shared<Node> (NodeType::ERROR));
				return;
			}

			// Break at a newline:
			unsigned i = 0;
			while (true) {
				if (m_lexer.la (i).type () == TokenType::WHITESPACE && m_lexer.la (i).hasLineBreak ()) {
					add (std::make_shared<Node> (NodeType::ERROR));
					return;
				}
				if (!isNonSignificant (m_lexer.la (i).type ())) {
					break;
				}
				++ i;
			}

			// Accept the next token in an error:
			{
				RuleScope scope (*this, NodeType::ERROR);

				accept ();

				add (scope.get ());
			}

			++ n;
		}
		accept ();
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

	Parser::Result Parser :: recover (std::initializer_list<TokenType> validTokenTypes) {
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
