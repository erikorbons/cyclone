#ifndef CYCLONE_PARSER_PARSER_H__
#define CYCLONE_PARSER_PARSER_H__

#include <string>
#include <memory>
#include <stack>
#include <cyclone/syntaxtree/Token.h>
#include <cyclone/syntaxtree/Node.h>
#include <cyclone/syntaxtree/CompilationUnit.h>
#include <cyclone/syntaxtree/Namespace.h>
#include <cyclone/syntaxtree/Using.h>
#include <cyclone/parser/Lexer.h>

namespace cyclone {
namespace parser {

class Parser;

namespace internal {

	class RuleScopeBase {
	public:

		typedef cyclone::syntaxtree::NonTerminalNode NonTerminalNode;

		RuleScopeBase (Parser & parser, const std::u16string & name, const std::shared_ptr<NonTerminalNode> target);
		~RuleScopeBase ();

		const std::u16string & name () const {
			return m_name;
		}

		NonTerminalNode * node () const {
			return m_target.get ();
		}

		std::shared_ptr<NonTerminalNode> get () const {
			return m_target;
		}

		Parser & parser () const {
			return m_parser;
		}

	private:

		Parser &												m_parser;
		std::u16string											m_name;
		std::shared_ptr<cyclone::syntaxtree::NonTerminalNode>	m_target;
	};
}

class Parser {
public:

	typedef cyclone::syntaxtree::Token				Token;
	typedef cyclone::syntaxtree::TokenType			TokenType;
	typedef cyclone::syntaxtree::TerminalNode		TerminalNode;
	typedef cyclone::syntaxtree::NonTerminalNode	NonTerminalNode;
	typedef cyclone::syntaxtree::CompilationUnit	CompilationUnit;
	typedef cyclone::syntaxtree::Namespace			Namespace;
	typedef cyclone::syntaxtree::Using				Using;

	template<class T> using Result = std::shared_ptr<T>;

	Parser (Lexer & lexer);

	Result<CompilationUnit> parseCompilationUnit ();
	Result<Namespace> parseNamespace ();
	Result<Using> parseUsing ();

private:

	friend class internal::RuleScopeBase;

	template<class T>
	class RuleScope : public internal::RuleScopeBase {
	public:

		RuleScope (Parser & parser, const std::u16string & name)
			: internal::RuleScopeBase (parser, name, std::static_pointer_cast<NonTerminalNode> (std::make_shared<T> ())) {
		}

		T * node () const {
			return (T *)internal::RuleScopeBase::node ();
		}

		std::shared_ptr<T> get () const {
			return std::static_pointer_cast<T> (internal::RuleScopeBase::get ());
		}
	};

	NonTerminalNode * node () const;

	static bool isNonSignificant (TokenType tokenType);

	bool check (TokenType tokenType);
	void expect (TokenType tokenType);
	void accept ();

	Lexer &									m_lexer;
	std::stack<internal::RuleScopeBase *>	m_scopes;
};

}	// namespace parser
}	// namespace cyclone

#endif	// CYCLONE_PARSER_PARSER_H__
