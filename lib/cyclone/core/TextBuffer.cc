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


		return TextBuffer ();
	}

} // namespace core
} // namespace cyclone
