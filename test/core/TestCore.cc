#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE	Cyclone Core
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(core)

BOOST_AUTO_TEST_CASE (my_test) {
	BOOST_TEST_MESSAGE ("Hello, World!");
	BOOST_CHECK (1 == 1);
}

BOOST_AUTO_TEST_SUITE_END()
