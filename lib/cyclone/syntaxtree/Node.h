#ifndef CYCLONE_SYNTAXTREE_NODE_H__
#define CYCLONE_SYNTAXTREE_NODE_H__

#include <memory>
#include <vector>
#include <cyclone/syntaxtree/Token.h>

namespace cyclone {
namespace syntaxtree {

enum class NodeType {
	ERROR,

	COMPILATION_UNIT,
	NAMESPACE,
	USING,
	SCOPED_NAME
};

class NodeBase {
public:

	virtual ~NodeBase () { }

	virtual std::size_t length () const = 0;
	virtual bool isTerminal () const = 0;
	virtual bool isNode () const = 0;
};

class Node : public NodeBase {
public:

	Node (NodeType type) : m_nodeType (type) {
	}

	virtual std::size_t length () const {
		return m_length;
	}

	void addNode (const std::shared_ptr<NodeBase> & node) {
		m_nodes.push_back (node);
		m_length += node->length ();
	}

	unsigned nodeCount () const {
		return m_nodes.size ();
	}

	std::shared_ptr<NodeBase> node (unsigned i) const {
		return m_nodes[i];
	}

	NodeType type () const {
		return m_nodeType;
	}

	virtual bool isTerminal () const { return false; }
	virtual bool isNode () const { return true; }

private:

	NodeType								m_nodeType;
	std::vector<std::shared_ptr<NodeBase>>	m_nodes;
	std::size_t								m_length;
};

class Terminal: public NodeBase {
public:

	Terminal (const Token & token) : m_token (token) {
	}

	virtual std::size_t length () const {
		return m_token.length ();
	}

	Token token () const {
		return m_token;
	}

	virtual bool isTerminal () const { return true; }
	virtual bool isNode () const { return false; }

private:

	Token	m_token;
};

}	// namespace syntaxtree
}	// namespace cyclone

#endif	// CYCLONE_SYNTAXTREE_NODE_H__
