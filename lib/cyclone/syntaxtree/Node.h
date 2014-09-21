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

class NonTerminalNode : public Node {
public:

	virtual std::size_t length () const {
		return m_length;
	}

	void addNode (const std::shared_ptr<Node> & node) {
		m_nodes.push_back (node);
		m_length += node->length ();
	}

private:

	std::vector<std::shared_ptr<Node>>	m_nodes;
	std::size_t							m_length;
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
