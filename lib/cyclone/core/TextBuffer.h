#ifndef CYCLONE_CORE_TEXTBUFFER_H
#define CYCLONE_CORE_TEXTBUFFER_H

#include <string>
#include <memory>

namespace cyclone {
namespace core {

namespace internal {
	class TextBufferNodeBase {
	public:

		virtual ~TextBufferNodeBase () { }

		virtual bool isSpan () const = 0;
		virtual bool isNode () const = 0;
		virtual std::size_t length () const = 0;
		virtual char16_t operator[] (std::size_t index) const = 0;
		virtual std::u16string toString () const = 0;
	};

	class TextBufferSpan : public TextBufferNodeBase {
	public:

		TextBufferSpan (const std::u16string & value) : m_value (value) {
		}

		virtual ~TextBufferSpan () { }

		virtual bool isSpan () const {
			return true;
		}

		virtual bool isNode () const {
			return false;
		}

		virtual std::size_t length () const {
			return m_value.length ();
		}

		virtual char16_t operator[] (std::size_t index) const {
			return m_value[index];
		}

		const std::u16string value () const {
			return m_value;
		}

		virtual std::u16string toString () const {
			return m_value;
		}

	private:

		std::u16string	m_value;
	};

	struct TextBufferNode : public TextBufferNodeBase {
	public:

		TextBufferNode (const std::shared_ptr<TextBufferNodeBase> & left, const std::shared_ptr<TextBufferNodeBase> & right)
			: m_left (left), m_right (right) {
		}

		virtual bool isSpan () const {
			return false;
		}

		virtual bool isNode () const {
			return true;
		}

		virtual std::size_t length () const {
			return m_left->length () + m_right->length ();
		}

		std::size_t leftLength () const {
			return m_left->length ();
		}

		virtual char16_t operator[] (std::size_t index) const {
			std::size_t leftLength = m_left->length ();

			if (index < leftLength) {
				return (*m_left)[index];
			} else {
				return (*m_right)[index - leftLength];
			}
		}

		const std::shared_ptr<TextBufferNodeBase> & left () const {
			return m_left;
		}

		const std::shared_ptr<TextBufferNodeBase> & right () const {
			return m_right;
		}

		virtual std::u16string toString () const {
			return m_left->toString () + m_right->toString ();
		}

	private:

		std::shared_ptr<TextBufferNodeBase>	m_left;
		std::shared_ptr<TextBufferNodeBase>	m_right;
	};

}

class TextBuffer {
private:

	typedef internal::TextBufferNodeBase	NodeBase;
	typedef internal::TextBufferNode		Node;
	typedef internal::TextBufferSpan		Span;

public:

	TextBuffer () : m_root (std::make_shared<Span> (u"")) {

	}

	TextBuffer (const std::u16string & value)
		: m_root (makeSpan (value)){

	}

	std::size_t length () const {
		return m_root->length ();
	}

	char16_t operator [] (std::size_t index) const {
		return (*m_root)[index];
	}

	TextBuffer splice (std::size_t offset, std::size_t length, const std::u16string & replacement) const;
	TextBuffer insert (std::size_t offset, const std::u16string & text) const;
	TextBuffer append (const TextBuffer & other) const;
	TextBuffer remove (std::size_t offset, std::size_t length) const;

	std::u16string toString () const {
		return m_root->toString ();
	}

private:

	const std::size_t	maxStringLength = 512;

	struct FindNode {
		FindNode (
				const std::shared_ptr<Span> & beforeNode,
				const std::shared_ptr<Span> & afterNode,
				std::size_t beforeOffset,
				std::size_t afterOffset)
				: m_beforeNode (beforeNode),
				  m_afterNode (afterNode),
				  m_beforeOffset (beforeOffset),
				  m_afterOffset (afterOffset) {
		}

		std::shared_ptr<Span>	m_beforeNode;
		std::shared_ptr<Span>	m_afterNode;
		std::size_t				m_beforeOffset;
		std::size_t				m_afterOffset;
	};

	struct Split {
		Split (const std::shared_ptr<NodeBase> & left, const std::shared_ptr<NodeBase> & right)
			: m_left (left), m_right (right) {
		}

		std::shared_ptr<NodeBase>	m_left;
		std::shared_ptr<NodeBase>	m_right;
	};

	TextBuffer (const std::shared_ptr<NodeBase> & root) : m_root (root) {
	}

	Split split (const std::shared_ptr<NodeBase> & node, std::size_t offset) const;
	Split combineSplitLeft (const std::shared_ptr<NodeBase> & a, const std::shared_ptr<NodeBase> & b, const std::shared_ptr<NodeBase> & c) const;
	Split combineSplitRight (const std::shared_ptr<NodeBase> & a, const std::shared_ptr<NodeBase> & b, const std::shared_ptr<NodeBase> & c) const;
	std::shared_ptr<NodeBase> makeSpan (const std::u16string & value) const;

	std::shared_ptr<NodeBase>	m_root;
};

} // namespace core
} // namespace cyclone

#endif // CYCLONE_CORE_TEXTBUFFER_H
