//
//Created by Hank 2016/3/11
//
#include "log.h"

HlsLog* HlsLog::instance_ = nullptr;

HlsLog::HlsLog()
{

}

HlsLog::~HlsLog()
{

}

HlsLog* HlsLog::getInstance()
{
	if(instance_ == nullptr)
		instance_ = new HlsLog();
	return instance_;
}

int HlsLog::log(string level_name,string tag, string log_message)
{
	cout<<"["<<level_name<<"]"<<"["<<tag<<"]"<<log_message<<endl;
	return log_message.size();
}
