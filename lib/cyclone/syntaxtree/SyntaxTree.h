#ifndef CYCLONE_SYNTAXTREE_SYNTAXTREE_H__
#define CYCLONE_SYNTAXTREE_SYNTAXTREE_H__

#include <memory>
#include <cyclone/syntaxtree/Node.h>

namespace cyclone {
namespace syntaxtree {

class SyntaxTree {
public:
	SyntaxTree (const std::shared_ptr<Node> & root) : m_root (root) {
	}

	std::shared_ptr<Node> root () const {
		return m_root;
	}

private:

	std::shared_ptr<Node>	m_root;
};

}
}
#endif	// CYCLONE_SYNTAXTREE_SYNTAXTREE_H__
