#ifndef CYCLONE_PARSER_PARSER_H__
#define CYCLONE_PARSER_PARSER_H__

#include <string>
#include <memory>
#include <stack>
#include <cyclone/syntaxtree/Token.h>
#include <cyclone/syntaxtree/Node.h>
#include <cyclone/parser/Lexer.h>

namespace cyclone {
namespace parser {

class Parser;

namespace internal {

	class RuleScope {
	public:

		typedef cyclone::syntaxtree::NodeType NodeType;
		typedef cyclone::syntaxtree::Node Node;

		RuleScope (Parser & parser, NodeType nodeType);
		~RuleScope ();

		Node * node () const {
			return m_target.get ();
		}

		std::shared_ptr<Node> get () const {
			return m_target;
		}

		Parser & parser () const {
			return m_parser;
		}

	private:

		Parser &				m_parser;
		std::shared_ptr<Node>	m_target;
	};
}

class Parser {
public:

	typedef cyclone::syntaxtree::Token		Token;
	typedef cyclone::syntaxtree::TokenType	TokenType;
	typedef cyclone::syntaxtree::NodeType	NodeType;
	typedef cyclone::syntaxtree::NodeBase		NodeBase;
	typedef cyclone::syntaxtree::Terminal	Terminal;
	typedef cyclone::syntaxtree::Node		Node;

	using Result = std::shared_ptr<Node>;

	Parser (Lexer & lexer);

	Result parseCompilationUnit ();
	Result parseNamespace ();
	Result parseUsing ();

private:

	friend class internal::RuleScope;
	typedef internal::RuleScope RuleScope;

	void parseNamespaceMember (bool inBlock);

	Node * node () const;

	static bool isNonSignificant (TokenType tokenType);

	bool check (TokenType tokenType);
	void expect (TokenType tokenType);
	void accept ();
	Result recover (std::initializer_list<TokenType> validTokenTypes);

	void add (const std::shared_ptr<NodeBase> & n) {
		node ()->addNode (n);
	}

	Lexer &									m_lexer;
	std::stack<internal::RuleScope *>		m_scopes;
};

}	// namespace parser
}	// namespace cyclone

#endif	// CYCLONE_PARSER_PARSER_H__
