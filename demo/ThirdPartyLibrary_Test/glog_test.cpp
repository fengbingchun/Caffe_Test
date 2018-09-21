#include "glog_test.hpp"
#include <iostream>
#include <glog/logging.h>
#include <boost/thread.hpp>

static void thread1_test()
{
	std::string strTmp = "thread1_test";
	for (int i = 0; i< 1000; i++) {
		//LOG(INFO) << i;
		LOG_IF(INFO, i < 10) << i;
		//CHECK_EQ(i, 100) << "error!";
		//LOG(INFO) << strTmp;
		//Sleep(10);
	}
}

static void thread2_test()
{
	std::string strTmp = "thread2_test";
	for (int i = 1000; i< 2000; i++) {
		//LOG(INFO) << i;
		LOG_IF(INFO, i < 1100) << i;
		//LOG(INFO) << strTmp;
		//Sleep(10);
	}
}

int test_glog_1()
{
	/* reference:
		http://www.yeolar.com/note/2014/12/20/glog/
		http://www.cppblog.com/pizzx/archive/2014/06/18/207320.aspx
	*/
#ifdef _MSC_VER
#ifdef _DEBUG
	const char* exe = "E:/GitCode/Caffe_Test/lib/dbg/x64_vc12/ThirdPartyLibrary_Test.exe";
#else
	const char* exe = "E:/GitCode/Caffe_Test/lib/rel/x64_vc12/ThirdPartyLibrary_Test.exe";
#endif
#else
	const char* exe = "build/ThridPartyLibrary_Test";
#endif
	//Initialize Google's logging library.
	//google::InitGoogleLogging(argv[0]);
	google::InitGoogleLogging(exe);

#ifdef _MSC_VER
	const std::string data_path = "E:/GitCode/Caffe_Test/test_data/third_party_library/";
#else
	const std::string data_path = "test_data/third_party_library/";
#endif

	//为不同级别的日志设置不同的文件basename
	std::string file_info = data_path + "loginfo";
	std::string file_logwarn = data_path + "logwarn";
	std::string file_logerror = data_path + "logerror";
	google::SetLogDestination(google::INFO, file_info.c_str());
	google::SetLogDestination(google::WARNING, file_logwarn.c_str());
	google::SetLogDestination(google::GLOG_ERROR, file_logerror.c_str());

	//缓存的最大时长，超时会写入文件
	FLAGS_logbufsecs = 60;

	//单个日志文件最大，单位M
	FLAGS_max_log_size = 10;

	//设置为true，就不会写日志文件了
	FLAGS_logtostderr = false;
	boost::thread t1(boost::bind(&thread1_test));
	boost::thread t2(boost::bind(&thread2_test));

	t1.join();
	t2.join();

	//LOG(FATAL)<<"exit";
	google::ShutdownGoogleLogging();

	return 0;
}
