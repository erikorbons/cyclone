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

		virtual char16_t operator[] (std::size_t index) const {
			std::size_t leftLength = m_left->length ();

			if (index < leftLength) {
				return (*m_left)[index];
			} else {
				return (*m_right)[index - leftLength];
			}
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
		: m_root (std::make_shared<Span> (value)){

	}

	std::size_t length () const {
		return m_root->length ();
	}

	char16_t operator [] (std::size_t index) const {
		return (*m_root)[index];
	}

	TextBuffer splice (std::size_t offset, std::size_t length, const std::u16string & replacement) const;

private:

	TextBuffer (const std::shared_ptr<NodeBase> & root) : m_root (root) {
	}

	std::shared_ptr<NodeBase>	m_root;
};

} // namespace core
} // namespace cyclone

#endif // CYCLONE_CORE_TEXTBUFFER_H
