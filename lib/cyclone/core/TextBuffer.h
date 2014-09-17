#ifndef CYCLONE_CORE_TEXTBUFFER_H
#define CYCLONE_CORE_TEXTBUFFER_H

#include <string>
#include <memory>

namespace cyclone {
namespace core {

namespace internal {
	class TextBufferNodeBase {
	public:

#ifdef TEXTBUFFER_DEBUG
		TextBufferNodeBase () { ++ m_nodeCount; }
#endif
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

	TextBufferIterator & operator ++ () { updateOffset (1); return *this; }
	TextBufferIterator operator ++ (int) { TextBufferIterator copy (*this); ++ copy; return copy; }

	TextBufferIterator & operator = (const TextBufferIterator & other) {
		m_buffer = other.m_buffer;
		m_currentSpan = other.m_currentSpan;
		m_offset = other.m_offset;
		m_spanOffset = other.m_spanOffset;
		return *this;
	}

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

} // namespace core
} // namespace cyclone

#endif // CYCLONE_CORE_TEXTBUFFER_H
