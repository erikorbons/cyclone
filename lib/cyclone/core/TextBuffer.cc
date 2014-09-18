#include <cyclone/core/TextBuffer.h>

namespace cyclone {
namespace core {

	TextBuffer TextBuffer :: splice (std::size_t offset, std::size_t length, const std::u16string & replacement) const {
		if (length == 0 && replacement.length () == 0) {
			// Nothing to do:
			return TextBuffer (m_root);
		}

		if (length == 0) {
			// Insert only:
			return insert (offset, replacement);
		}

		Split leftSplit = split (m_root, offset);
		Split rightSplit = leftSplit.m_right == nullptr ? Split (nullptr, nullptr) : split (leftSplit.m_right, length);

		// Delete
		if (replacement.length () == 0) {
			if (leftSplit.m_left == nullptr) {
				return rightSplit.m_right == nullptr ? TextBuffer () : TextBuffer (rightSplit.m_right);
			} else if (rightSplit.m_right == nullptr) {
				return leftSplit.m_left == nullptr ? TextBuffer () : TextBuffer (leftSplit.m_left);
			} else {
				return TextBuffer (leftSplit.m_left).append (TextBuffer (rightSplit.m_right));
			}
		}

		// Insert:
		if (leftSplit.m_left == nullptr && rightSplit.m_right == nullptr) {
			return TextBuffer (replacement);
		} else if (leftSplit.m_left == nullptr) {
			return TextBuffer (replacement).append (TextBuffer (rightSplit.m_right));
		} else if (rightSplit.m_right == nullptr) {
			return TextBuffer (leftSplit.m_left).append (TextBuffer (replacement));
		} else {
			return TextBuffer (leftSplit.m_left)
				.append (TextBuffer (replacement))
				.append (TextBuffer (rightSplit.m_right));
		}
	}

	TextBuffer TextBuffer :: insert (std::size_t offset, const std::u16string & text) const {
		Split splitResult = split (m_root, offset);

		// Special cases, prepend or append:
		if (splitResult.m_left == nullptr) {
			return TextBuffer (text).append (TextBuffer (splitResult.m_right));
		} else if (splitResult.m_right == nullptr) {
			return TextBuffer (splitResult.m_left).append (TextBuffer (text));
		}

		return TextBuffer (splitResult.m_left).append (TextBuffer (text)).append (TextBuffer (splitResult.m_right));
	}

	TextBuffer TextBuffer :: append (const TextBuffer & other) const {
		std::shared_ptr<Node> newRoot = std::make_shared<Node> (
			m_root,
			other.m_root
		);

		while (true) {
			int leftDepth = newRoot->left ()->depth ();
			int rightDepth = newRoot->right ()->depth ();

			if (rightDepth - leftDepth >= 2) {
				// Right is deeper than left:
				newRoot = rotateLeft (newRoot);
			} else if (leftDepth - rightDepth >= 2) {
				// Left is deeper than right:
				newRoot = rotateRight (newRoot);
			} else {
				break;
			}
		}

		return TextBuffer (std::static_pointer_cast<NodeBase> (newRoot));
	}

	TextBuffer TextBuffer :: remove (std::size_t offset, std::size_t length) const {
		return splice (offset, length, u"");
	}

	TextBuffer::Split TextBuffer :: split (const std::shared_ptr<NodeBase> & node, std::size_t offset) const {
		// Split spans:
		if (node->isSpan ()) {
			if (offset == 0) {
				// The split is at the beginning of the node:
				return Split (nullptr, node);
			} else if (offset >= node->length ()) {
				// The split is at the end of the node:
				return Split (node, nullptr);
			} else {
				// Split this span:
				std::shared_ptr<Span> span = std::static_pointer_cast<Span> (node);
				return Split (
					makeSpan (span->value ().substr (0, offset)),
					makeSpan (span->value ().substr (offset))
				);
			}
		}

		// Split internal nodes:
		std::shared_ptr<Node> n = std::static_pointer_cast<Node> (node);
		std::size_t leftLength = n->leftLength ();

		if (offset < leftLength) {
			Split s = split (n->left (), offset);
			return combineSplitLeft (s.m_left, s.m_right, n->right ());
		} else {
			Split s = split (n->right (), offset - leftLength);
			return combineSplitRight (n->left (), s.m_left, s.m_right);
		}
	}

	TextBuffer::Split TextBuffer :: combineSplitLeft (const std::shared_ptr<NodeBase> & a, const std::shared_ptr<NodeBase> & b, const std::shared_ptr<NodeBase> & c) const {
		if (a == nullptr) {
			if (b == nullptr) {
				return Split (nullptr, c);
			} else {
				return Split (nullptr, std::static_pointer_cast<NodeBase> (
					std::make_shared<Node> (b, c)
				));
			}
		} else {
			if (b == nullptr) {
				return Split (a, c);
			} else {
				return Split (a, std::static_pointer_cast<NodeBase> (
					std::make_shared<Node> (b, c)
				));
			}
		}
	}

	TextBuffer::Split TextBuffer :: combineSplitRight (const std::shared_ptr<NodeBase> & a, const std::shared_ptr<NodeBase> & b, const std::shared_ptr<NodeBase> & c) const {
		if (c == nullptr) {
			if (b == nullptr) {
				return Split (a, nullptr);
			} else {
				return Split (std::static_pointer_cast<NodeBase> (
					std::make_shared<Node> (a, b)
				), nullptr);
			}
		} else {
			if (b == nullptr) {
				return Split (a, c);
			} else {
				return Split (std::static_pointer_cast<NodeBase> (
					std::make_shared<Node> (a, b)
				), c);
			}
		}
	}

	std::shared_ptr<TextBuffer::NodeBase> TextBuffer :: makeSpan (const std::u16string & value) const {
		if (value.length () <= maxStringLength) {
			return std::static_pointer_cast<NodeBase> (
				std::make_shared<Span> (value)
			);
		}

		return std::static_pointer_cast<NodeBase> (
			std::make_shared<Node> (
				makeSpan (value.substr (0, value.length () / 2)),
				makeSpan (value.substr (value.length () / 2))
			)
		);
	}

	std::shared_ptr<TextBuffer::Node> TextBuffer :: rotateLeft (const std::shared_ptr<Node> node) const {
		// Cannot rotate left if there is a span at the right:
		if (node->right ()->isSpan ()) {
			return node;
		}

		std::shared_ptr<Node> right = std::static_pointer_cast<Node> (node->right ());

		std::shared_ptr<Node> newLeft = std::make_shared<Node> (
			node->left (),
			right->left ()
		);

		return std::make_shared<Node> (
			newLeft,
			right->right ()
		);
	}

	std::shared_ptr<TextBuffer::Node> TextBuffer :: rotateRight (const std::shared_ptr<Node> node) const {
		// Cannot rotate right if there is a span at the left:
		if (node->left ()->isSpan ()) {
			return node;
		}

		std::shared_ptr<Node> left = std::static_pointer_cast<Node> (node->left ());

		std::shared_ptr<Node> newRight = std::make_shared<Node> (
			left->right (),
			node->right ()
		);

		return std::make_shared<Node> (
			left->left (),
			newRight
		);
	}

	void TextBufferIterator :: setOffset (std::size_t offset) {
		std::shared_ptr<NodeBase> currentNode = m_buffer->m_root;
		std::size_t currentOffset = offset;

		while (!currentNode->isSpan ()) {
			std::shared_ptr<Node> node = std::static_pointer_cast<Node> (currentNode);
			std::size_t leftLength = node->left ()->length ();

			if (currentOffset < leftLength) {
				currentNode = node->left ();
			} else {
				currentNode = node->right ();
				currentOffset -= leftLength;
			}
		}

		m_currentSpan = std::static_pointer_cast<Span> (currentNode).get ();
		m_spanOffset = currentOffset;
		m_offset = offset;
	}

} // namespace core
} // namespace cyclone
