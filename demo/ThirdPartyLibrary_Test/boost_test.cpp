#include "boost_test.hpp"
#include <iostream>
#include <boost/shared_ptr.hpp>

class implementation {
public:
	~implementation() { std::cout << "destroying implementation\n"; }
	void do_something() { std::cout << "do something\n"; }
};

int test_boost_shared_ptr()
{
	// reference: http://www.cnblogs.com/tianfang/archive/2008/09/19/1294521.html
	boost::shared_ptr<implementation> sp1(new implementation());
	std::cout << "The Sample now has " << sp1.use_count() << " references\n";

	boost::shared_ptr<implementation> sp2 = sp1;
	std::cout << "The Sample now has " << sp2.use_count() << " references\n";

	sp1.reset();
	std::cout << "After Reset sp1. The Sample now has " << sp2.use_count() << " references\n";

	sp2.reset();
	std::cout << "After Reset sp2. The Sample now has " << sp2.use_count() << " references\n";

	return 0;
}

