#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE	Cyclone Parser
#include <boost/test/unit_test.hpp>

#include <iostream>
#include <cyclone/core/TextBuffer.h>
#include <cyclone/parser/Lexer.h>
#include <cyclone/parser/Parser.h>
#include <utf8/utf8.h>


using namespace cyclone::core;
using namespace cyclone::syntaxtree;
using namespace cyclone::parser;

static Lexer mkLexer (const std::u16string & value) {
	TextBuffer buffer (value);
	return Lexer (buffer, buffer.begin (), buffer.end ());
}

static std::string convert (const std::u16string & input) {
	std::string result;
	utf8::utf16to8 (input.begin (), input.end (), back_inserter (result));
	return result;
}

static void printTree (const TextBuffer & buffer, const std::shared_ptr<NodeBase> & node, unsigned offset = 0, int indent = 0) {
	for (int i = 0; i < indent; ++ i) {
		std::cout << " ";
	}

	if (node->isTerminal ()) {
		std::shared_ptr<Terminal> terminal = std::static_pointer_cast<Terminal> (node);
		std::cout << "TERMINAL " << offset << " - " << (offset + terminal->length ()) << ": ";
		std::u16string s;
		TextBuffer::Iterator i = buffer.at (offset);
		TextBuffer::Iterator end = buffer.at (offset + terminal->length ());
		for (; i != end; ++ i) {
			if (*i == '\r') {
				s += u"\\r";
			} else if (*i == '\n') {
				s += u"\\n";
			} else {
				s += *i;
			}
		}
		if (terminal->token ().type () == TokenType::END_OF_INPUT) {
			s += u"<END_OF_INPUT>";
		}
		std::cout << "\"" << convert (s) << "\"" << std::endl;
	} else {
		std::shared_ptr<Node> n = std::static_pointer_cast<Node> (node);
		switch (n->type ()) {
		case NodeType::ERROR:
			std::cout << "ERROR";
			break;
		case NodeType::COMPILATION_UNIT:
			std::cout << "COMPILATION_UNIT";
			break;
		case NodeType::NAMESPACE:
			std::cout << "NAMESPACE";
			break;
		case NodeType::USING:
			std::cout << "USING";
			break;
		case NodeType::SCOPED_NAME:
			std::cout << "SCOPED_NAME";
			break;
		}
		std::cout << std::endl;

		for (unsigned i = 0; i < n->nodeCount (); ++ i) {
			printTree (buffer, n->node (i), offset, indent + 2);
			offset += n->node (i)->length ();
		}
	}
}

BOOST_AUTO_TEST_SUITE(TestParser)

BOOST_AUTO_TEST_CASE (my_test) {
	BOOST_TEST_MESSAGE ("Hello, World!");
	BOOST_CHECK (1 == 1);
}

BOOST_AUTO_TEST_CASE (testCreateParser) {
	Lexer l = mkLexer (u"");
	Parser p (l);
}

BOOST_AUTO_TEST_CASE (testParseCompilationUnit) {
	{
		Lexer l = mkLexer (u"  ");
		Parser p (l);

		Parser::Result cu = p.parseCompilationUnit ();

		BOOST_CHECK (cu->nodeCount () == 2);
		BOOST_CHECK (cu->length () == 2);
	}

	{
		TextBuffer buffer (u"using a.b; /* bla */ namespace c { namespace d { } }");
		Lexer l (buffer, buffer.begin (), buffer.end ());
		Parser p (l);

		Parser::Result cu = p.parseCompilationUnit ();

		printTree (buffer, cu);
	}
}

BOOST_AUTO_TEST_SUITE_END()
