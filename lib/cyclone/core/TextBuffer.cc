#include <cyclone/core/TextBuffer.h>

namespace cyclone {
namespace core {

	TextBuffer TextBuffer :: splice (std::size_t offset, std::size_t length, const std::u16string & replacement) const {
		std::size_t	currentLength = this->length ();

		// Special case, prepend:
		if (offset == 0 && length == 0) {
			if (replacement.length () == 0) {
				return TextBuffer (m_root);
			} else {
				return TextBuffer (std::make_shared<Node> (
					std::make_shared<Span> (replacement),
					m_root
				));
			}
		}

		// Special case, append:
		if (offset == currentLength) {
			if (replacement.length () == 0) {
				return TextBuffer (m_root);
			} else {
				return TextBuffer (std::make_shared<Node> (
					m_root,
					std::make_shared<Span> (replacement)
				));
			}
		}

		// Special case: no-op splice.
		if (length == 0 && replacement.length () == 0) {
			return TextBuffer (m_root);
		}

		FindNode start = findNode (offset);
		FindNode end = findNode (offset + length);

		// Special case: insert / delete within the same node.
		if (start.m_afterNode == end.m_beforeNode) {
			return TextBuffer (spliceInNode (m_root, offset, length, replacement));
		}

		return TextBuffer ();
	}

	std::shared_ptr<TextBuffer::NodeBase> TextBuffer :: spliceInNode (const std::shared_ptr<NodeBase> & node, std::size_t offset, std::size_t length, const std::u16string & replacement) const {
		if (!node->isSpan ()) {
			std::shared_ptr<Node> n = std::static_pointer_cast<Node> (node);

			std::size_t leftLength = n->left ()->length ();

			if (offset < leftLength) {
				std::shared_ptr<NodeBase> newLeft = spliceInNode (n->left (), offset, length, replacement);
				return std::static_pointer_cast<NodeBase> (
					std::make_shared<Node> (newLeft, n->right ())
				);
			} else {
				std::shared_ptr<NodeBase> newRight = spliceInNode (n->right (), offset - leftLength, length, replacement);
				return std::static_pointer_cast<NodeBase> (
					std::make_shared<Node> (n->left (), newRight)
				);
			}
		}

		std::shared_ptr<Span> span = std::static_pointer_cast<Span> (node);

		// Choose between replacing the span or splitting it.
		if (span->length () < splitThreshold) {
			// Allocate a new span:
			std::u16string newValue (span->value ());

			newValue.replace (offset, length, replacement);

			return std::static_pointer_cast<NodeBase> (
				std::make_shared<Span> (std::move (newValue))
			);
		} else if (offset == 0 && length == 0) {
			// Prepend:
			return std::static_pointer_cast<NodeBase> (
				std::make_shared<Node> (
					std::static_pointer_cast<NodeBase> (std::make_shared<Span> (replacement)),
					std::static_pointer_cast<NodeBase> (span)
				)
			);
		} else if (offset == 0) {
			// Replace from the beginning:
			return std::static_pointer_cast<NodeBase> (
				std::make_shared<Node> (
					std::static_pointer_cast<NodeBase> (std::make_shared<Span> (replacement)),
					std::static_pointer_cast<NodeBase> (std::make_shared<Span> (span->value ().substr (offset)))
				)
			);
		} else if (offset == span->length ()) {
			// Append:
			return std::static_pointer_cast<NodeBase> (
				std::make_shared<Node> (
					std::static_pointer_cast<NodeBase> (span),
					std::static_pointer_cast<NodeBase> (std::make_shared<Span> (replacement))
				)
			);
		} else if (replacement.length () == 0) {
			// Delete characters in the middle:
			return std::static_pointer_cast<NodeBase> (
				std::make_shared<Node> (
					std::static_pointer_cast<NodeBase> (std::make_shared<Span> (span->value ().substr (0, offset))),
					std::static_pointer_cast<NodeBase> (std::make_shared<Span> (span->value ().substr (offset)))
				)
			);
		} else {
			// Splice in the middle:
			return std::static_pointer_cast<NodeBase> (
				std::make_shared<Node> (
					std::static_pointer_cast<NodeBase> (
						std::make_shared<Node> (
							std::static_pointer_cast<NodeBase> (std::make_shared<Span> (span->value ().substr (0, offset))),
							std::static_pointer_cast<NodeBase> (std::make_shared<Span> (replacement))
						)
					),
					std::static_pointer_cast<NodeBase> (std::make_shared<Span> (span->value ().substr (offset + length)))
				)
			);
		}
	}

	TextBuffer::FindNode TextBuffer :: findNode (std::size_t offset) const {
		std::size_t beforeOffset = offset;
		std::shared_ptr<NodeBase> beforeNode = m_root;

		while (!beforeNode->isSpan ()) {
			std::shared_ptr<Node> n = std::static_pointer_cast<Node> (beforeNode);

			std::size_t leftLength = n->left ()->length ();

			if (beforeOffset <= leftLength) {
				beforeNode = std::static_pointer_cast<NodeBase> (n->left ());
			} else {
				beforeNode = std::static_pointer_cast<NodeBase> (n->right ());
				beforeOffset = beforeOffset - leftLength;
			}
		}

		// If the split point is in the middle of a span, return the same node for both
		// start and end:
		if (beforeOffset < beforeNode->length ()) {
			return FindNode (
					std::static_pointer_cast<Span> (beforeNode),
					std::static_pointer_cast<Span> (beforeNode),
					beforeOffset,
					beforeOffset
				);
		}

		// Locate the end span:
		std::shared_ptr<NodeBase> afterNode = m_root;
		std::size_t afterOffset = offset;
		while (!afterNode->isSpan ()) {
			std::shared_ptr<Node> n = std::static_pointer_cast<Node> (afterNode);

			std::size_t leftLength = n->left ()->length ();

			if (beforeOffset < leftLength) {
				afterNode = std::static_pointer_cast<NodeBase> (n->left ());
			} else {
				afterNode = std::static_pointer_cast<NodeBase> (n->right ());
				afterOffset = afterOffset - leftLength;
			}
		}

		return FindNode (
			std::static_pointer_cast<Span> (beforeNode),
			std::static_pointer_cast<Span> (afterNode),
			beforeOffset,
			afterOffset
		);
	}

} // namespace core
} // namespace cyclone
