#ifndef CYCLONE_CORE_TEXTBUFFER_H
#define CYCLONE_CORE_TEXTBUFFER_H

#include <string>
#include <memory>

namespace cyclone {
namespace core {

namespace internal {
	class TextBufferNodeBase {
	public:

		TextBufferNodeBase () {
#ifdef TEXTBUFFER_DEBUG
		++ m_nodeCount;
#endif
		}

		virtual ~TextBufferNodeBase () {
#ifdef TEXTBUFFER_DEBUG
			-- m_nodeCount;
#endif
		}

		virtual bool isSpan () const = 0;
		virtual bool isNode () const = 0;
		virtual std::size_t length () const = 0;
		virtual char16_t operator[] (std::size_t index) const = 0;
		virtual std::u16string toString () const = 0;
		virtual int depth () const = 0;

#ifdef TEXTBUFFER_DEBUG
		static int m_nodeCount;
#endif
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

		virtual int depth () const {
			return 1;
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

		virtual int depth () const {
			int ld = m_left->depth ();
			int rd = m_right->depth ();

			return ld > rd ? ld : rd;
		}

		virtual std::u16string toString () const {
			return m_left->toString () + m_right->toString ();
		}

	private:

		std::shared_ptr<TextBufferNodeBase>	m_left;
		std::shared_ptr<TextBufferNodeBase>	m_right;
	};

}

class TextBufferIterator;

class TextBuffer {
private:

	typedef internal::TextBufferNodeBase	NodeBase;
	typedef internal::TextBufferNode		Node;
	typedef internal::TextBufferSpan		Span;

public:

	typedef TextBufferIterator				Iterator;

	TextBuffer () : m_root (std::make_shared<Span> (u"")) {
	}

	TextBuffer (const std::u16string & value)
		: m_root (makeSpan (value)){
	}

	TextBuffer (const TextBuffer & other) : m_root (other.m_root) {
	}

	TextBuffer & operator = (const TextBuffer & other) {
		m_root = other.m_root;
		return *this;
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

	TextBufferIterator begin () const;
	TextBufferIterator end () const;
	TextBufferIterator at (std::size_t offset) const;

	bool isBalanced () const {
		if (m_root->isSpan ()) {
			return true;
		}

		std::shared_ptr<Node> node = std::static_pointer_cast<Node> (m_root);
		int ld = node->left ()->depth ();
		int rd = node->right ()->depth ();

		return ld == rd || ld - rd == 1 || rd - ld == 1;
	}

	int depth () const {
		return m_root->depth ();
	}

	std::u16string toString () const {
		return m_root->toString ();
	}

private:

	friend class TextBufferIterator;

	const std::size_t	maxStringLength = 512;

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
	std::shared_ptr<Node> rotateLeft (const std::shared_ptr<Node> n) const;
	std::shared_ptr<Node> rotateRight (const std::shared_ptr<Node> n) const;

	std::shared_ptr<NodeBase>	m_root;
};

class TextBufferIterator {
private:

	typedef internal::TextBufferNodeBase	NodeBase;
	typedef internal::TextBufferNode		Node;
	typedef internal::TextBufferSpan		Span;

public:

	TextBufferIterator () : m_buffer (0), m_currentSpan (0), m_offset (0), m_spanOffset (0) {
	}

	TextBufferIterator (const TextBuffer & buffer, std::size_t offset) : m_buffer (&buffer) {
		setOffset (offset);
	}

	TextBufferIterator (const TextBufferIterator & other)
		: m_buffer (other.m_buffer), m_currentSpan (other.m_currentSpan), m_offset (other.m_offset), m_spanOffset (other.m_spanOffset) {
	}

	TextBufferIterator & operator = (const TextBufferIterator & other) {
		m_buffer = other.m_buffer;
		m_currentSpan = other.m_currentSpan;
		m_offset = other.m_offset;
		m_spanOffset = other.m_spanOffset;
		return *this;
	}

	TextBufferIterator & operator ++ () { updateOffset (1); return *this; }
	TextBufferIterator operator ++ (int) { TextBufferIterator copy (*this); ++ copy; return copy; }

	bool operator == (const TextBufferIterator & other) const {
		return m_buffer->m_root == other.m_buffer->m_root && m_offset == other.m_offset;
	}

	bool operator != (const TextBufferIterator & other) const {
		return !((*this) == other);
	}

	bool operator < (const TextBufferIterator & other) const {
		return m_offset < other.m_offset;
	}

	bool operator <= (const TextBufferIterator & other) const {
		return m_offset <= other.m_offset;
	}

	bool operator > (const TextBufferIterator & other) const {
		return m_offset > other.m_offset;
	}

	bool operator >= (const TextBufferIterator & other) const {
		return m_offset >= other.m_offset;
	}

	char16_t operator * () const {
		return m_currentSpan->value ()[m_spanOffset];
	}

	std::size_t offset () const {
		return m_offset;
	}

private:

	void setOffset (std::size_t offset);

	void updateOffset (int delta) {
		// Navigate to a new span:
		if (signed (m_spanOffset) + delta < 0 || signed (m_spanOffset) + delta >= signed (m_currentSpan->length ())) {
			setOffset (m_offset + delta);
			return;
		}

		// Navigate within the current span:
		m_offset += delta;
		m_spanOffset += delta;
	}

	const TextBuffer *	m_buffer;
	const Span *		m_currentSpan;
	std::size_t			m_offset;
	std::size_t			m_spanOffset;
};

inline TextBuffer::Iterator TextBuffer :: begin () const {
	return Iterator (*this, 0);
}

inline TextBuffer::Iterator TextBuffer :: end () const {
	return Iterator (*this, length ());
}

inline TextBuffer::Iterator TextBuffer :: at (std::size_t offset) const {
	return Iterator (*this, offset);
}


} // namespace core
} // namespace cyclone

#endif // CYCLONE_CORE_TEXTBUFFER_H
