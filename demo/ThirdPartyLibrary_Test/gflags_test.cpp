#include "gflags_test.hpp"
#include <iostream>
#include <gflags/gflags.h>

DEFINE_bool(big_menu, true, "Include 'advanced' options in the menu listing");
DEFINE_string(languages, "english,french,german", "comma-separated list of languages to offer in the 'lang' menu");

int test_gflags_1()
{
	/* reference:
		http://dreamrunner.org/blog/2014/03/09/gflags-jian-ming-shi-yong/
		http://www.leoox.com/?p=270
	*/
	int tmp_argc = 3;
	char** tmp_argv = NULL;
	tmp_argv = new char*[3];
	tmp_argv[0] = "";
	tmp_argv[1] = "--big_menu=false";
	tmp_argv[2] = "--languages=chinese";

	//google::ParseCommandLineFlags(&argc, &argv, true);
	google::ParseCommandLineFlags(&tmp_argc, &tmp_argv, true);

	std::cout << "argc=" << tmp_argc << std::endl;
	if (FLAGS_big_menu) {
		std::cout << "big menu is ture" << std::endl;
	}
	else {
		std::cout << "big menu is flase" << std::endl;
	}

	std::cout << "languages=" << FLAGS_languages << std::endl;

	return 0;
}

