#ifndef CYCLONE_SYNTAXTREE_NODE_H__
#define CYCLONE_SYNTAXTREE_NODE_H__

#include <memory>
#include <vector>
#include <cyclone/syntaxtree/Token.h>

namespace cyclone {
namespace syntaxtree {

class Node {
public:

	virtual ~Node () { }

	virtual std::size_t length () const = 0;
};

class NonterminalNode : public Node {
public:

private:

	std::vector<std::shared_ptr<Node>>	m_nodes;
};

class TerminalNode: public Node {
public:

	TerminalNode (const Token & token) : m_token (token) {
	}

	virtual std::size_t length () const {
		return m_token.length ();
	}

private:

	Token	m_token;
};

}	// namespace syntaxtree
}	// namespace cyclone

#endif	// CYCLONE_SYNTAXTREE_NODE_H__
