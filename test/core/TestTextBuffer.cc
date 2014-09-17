#include <boost/test/unit_test.hpp>

#include <fstream>
#include <string>
#include <locale>
#include <cyclone/core/TextBuffer.h>
#include <utf8/utf8.h>

using namespace cyclone :: core;

std::string convert (const std::u16string & input) {
	std::string result;
	utf8::utf16to8 (input.begin (), input.end (), back_inserter (result));
	return result;
}

bool assertTextBufferContent (const TextBuffer & buffer, const char16_t * content) {
	std::u16string s (content);

	if (s.length () != buffer.length ()) {
		return false;
	}

	for (std::size_t i = 0; i < s.length (); ++ i) {
		if (s[i] != buffer[i]) {
			return false;
		}
	}

	return true;
}

BOOST_AUTO_TEST_SUITE (TestTextBuffer)

BOOST_AUTO_TEST_CASE (testCreate) {

	TextBuffer buffer;

	BOOST_CHECK (buffer.length () == 0);
	BOOST_CHECK (assertTextBufferContent (buffer, u""));

	TextBuffer buffer2 (u"Hello, World!");

	BOOST_CHECK (buffer2.length () == 13);
	BOOST_CHECK (assertTextBufferContent (buffer2, u"Hello, World!"));
}

BOOST_AUTO_TEST_CASE (testPrepend) {
	TextBuffer original (u"Hello, World!");
	TextBuffer prepended = original.splice (0, 0, u"abcd ");

	BOOST_CHECK_MESSAGE (assertTextBufferContent (prepended, u"abcd Hello, World!"), convert (prepended.toString ()));

	TextBuffer prepended2 = prepended.splice (0, 0, u"efgh");

	BOOST_CHECK_MESSAGE (assertTextBufferContent (prepended2, u"efghabcd Hello, World!"), convert (prepended2.toString ()));

	TextBuffer prepended3 = prepended2.splice (0, 0, u"");

	BOOST_CHECK_MESSAGE (assertTextBufferContent (prepended3, u"efghabcd Hello, World!"), convert (prepended3.toString ()));
}

BOOST_AUTO_TEST_CASE (testAppend) {
	TextBuffer original (u"Hello, World!");
	TextBuffer appended = original.splice (original.length (), 0, u" abcd");

	BOOST_CHECK (assertTextBufferContent (appended, u"Hello, World! abcd"));

	TextBuffer appended2 = appended.splice (appended.length (), 0, u"efgh");

	BOOST_CHECK (assertTextBufferContent (appended2, u"Hello, World! abcdefgh"));

	TextBuffer appended3 = appended2.splice (appended2.length (), 0, u"");

	BOOST_CHECK (assertTextBufferContent (appended3, u"Hello, World! abcdefgh"));
}

BOOST_AUTO_TEST_CASE (testInsert) {
	TextBuffer original (u"abcdefgh");
	TextBuffer inserted = original.splice (4, 0, u"1234");

	BOOST_CHECK (assertTextBufferContent (inserted, u"abcd1234efgh"));

	TextBuffer inserted2 = inserted.splice (8, 0, u"5678");

	BOOST_CHECK (assertTextBufferContent (inserted2, u"abcd12345678efgh"));

	TextBuffer inserted3 = inserted2.splice (10, 0, u"xx");

	BOOST_CHECK_MESSAGE (assertTextBufferContent (inserted3, u"abcd123456xx78efgh"), convert (inserted3.toString ()));
}

BOOST_AUTO_TEST_CASE (testDelete) {
	TextBuffer original (u"abcd12345678efgh");
	TextBuffer deleted = original.splice (6, 4, u"");

	BOOST_CHECK (assertTextBufferContent (deleted, u"abcd1278efgh"));

	TextBuffer deleted2 = deleted.splice (4, 4, u"");

	BOOST_CHECK_MESSAGE (assertTextBufferContent (deleted2, u"abcdefgh"), convert (deleted2.toString ()));

	TextBuffer deleted3 = deleted2.splice (0, 1, u"");

	BOOST_CHECK_MESSAGE (assertTextBufferContent (deleted3, u"bcdefgh"), convert (deleted3.toString ()));

	TextBuffer deleted4 = deleted3.splice (6, 1, u"");

	BOOST_CHECK_MESSAGE (assertTextBufferContent (deleted4, u"bcdefg"), convert (deleted4.toString ()));
}

BOOST_AUTO_TEST_CASE (testReplace) {
	TextBuffer original (u"abcd12345678efgh");
	TextBuffer replaced = original.splice (6, 4, u"xxxx");

	BOOST_CHECK_MESSAGE (assertTextBufferContent (replaced, u"abcd12xxxx78efgh"), convert (replaced.toString ()));

	TextBuffer replaced2 = replaced.splice (4, 4, u"ABCD");

	BOOST_CHECK_MESSAGE (assertTextBufferContent (replaced2, u"abcdABCDxx78efgh"), convert (replaced2.toString ()));

	TextBuffer replaced3 = replaced2.splice (0, 1, u"ZZ");

	BOOST_CHECK_MESSAGE (assertTextBufferContent (replaced3, u"ZZbcdABCDxx78efgh"), convert (replaced3.toString ()));

	TextBuffer replaced4 = replaced3.splice (replaced3.length () - 1, 1, u"YY");

	BOOST_CHECK_MESSAGE (assertTextBufferContent (replaced4, u"ZZbcdABCDxx78efgYY"), convert (replaced4.toString ()));
}

BOOST_AUTO_TEST_SUITE_END ()
