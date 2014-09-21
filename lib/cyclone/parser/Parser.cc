#include <cyclone/parser/Parser.h>

namespace cyclone {
namespace parser {

	namespace internal {
		RuleScopeBase :: RuleScopeBase (Parser & parser, const std::u16string & name, const std::shared_ptr<NonTerminalNode> target)
			: m_parser (parser),
			  m_name (name),
			  m_target (target) {
			m_parser.m_scopes.push (this);
		}

		RuleScopeBase :: ~RuleScopeBase () {
			m_parser.m_scopes.pop ();
		}

	}

	Parser :: Parser (Lexer & lexer) : m_lexer (lexer) {
	}

	Parser::Result<Parser::CompilationUnit> Parser :: parseCompilationUnit () {
		RuleScope<CompilationUnit> scope (*this, u"compilationUnit");

		return scope.get ();
	}

	Parser::Result<Parser::Namespace> Parser :: parseNamespace () {
		RuleScope<Namespace> scope (*this, u"namespace");

		return scope.get ();
	}

	Parser::Result<Parser::Using> Parser :: parseUsing () {
		RuleScope<Using> scope (*this, u"using");

		return scope.get ();
	}

	bool Parser :: isNonSignificant (TokenType tokenType) {
		return tokenType == TokenType :: WHITESPACE
			|| tokenType == TokenType :: SINGLE_LINE_COMMENT
			|| tokenType == TokenType :: MULTI_LINE_COMMENT
			|| tokenType == TokenType :: INVALID_CHARACTERS;
	}

	Parser::NonTerminalNode * Parser :: node () const {
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
			node ()->addNode (std::make_shared<TerminalNode> (token));
			if (!isNonSignificant (token.type ())) {
				break;
			}
		}
	}
}
}
